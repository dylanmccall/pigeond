#include "linkmod_files.h"

#include "../util.h"
#include "../base64.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: It would be way better to use udisk for this, since it provides event
//       callbacks for directories being mounted. Alas, opendir is much, much
//       easier to throw together on a whim in our case :)

// FIXME: There is currently no way to stop this from writing to files.
//        Perhaps stop automatically and beep after 20 packets or 10 seconds?

// TODO: It is only possible to have frames.tx or frames.rx in a single place.
//       They can never overlap. We should have a mechanism so a device can
//       both read and write frames without getting confused.

#define FILES_DIR_TX_VAR_NAME "PIGEOND_FILES_TX"
#define FILES_DIR_RX_VAR_NAME "PIGEOND_FILES_RX"
#define FRAMES_TX_FILE_NAME ".pigeond.frames.tx"

#define MAX_WRITE_COUNT 20
#define MAX_WRITE_TIME 10

#define READ_BUFFER_SIZE PIGEON_LINK_MTU * 10

static struct timespec FILES_DIR_POLL_DELAY = {
	.tv_sec=1,
	.tv_nsec=0
};

typedef struct {
	PigeonLinkmod public;
	const char *files_dir_path;
	char *frames_file_path;
	Base64 *base64;
} LinkmodFiles;

bool _linkmod_files_thread_start(LongThread *long_thread, void *data);
bool _linkmod_files_thread_stop(LongThread *long_thread, void *data);

LongThreadResult _linkmod_files_tx_thread_loop(LongThread *long_thread, void *data);
LongThreadResult _linkmod_files_rx_thread_loop(LongThread *long_thread, void *data);

bool _push_to_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link);
bool _pop_from_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link);

bool linkmod_files_tx_is_available() {
	return getenv(FILES_DIR_TX_VAR_NAME) != NULL;
}

PigeonLinkmod *linkmod_files_tx_new() {
	LinkmodFiles *linkmod_files = malloc(sizeof(LinkmodFiles));
	memset(linkmod_files, 0, sizeof(*linkmod_files));
	linkmod_files->base64 = base64_new();
	linkmod_files->files_dir_path = getenv(FILES_DIR_TX_VAR_NAME);
	linkmod_files->public.long_thread = long_thread_new((LongThreadOptions){
		.name="linkmod-console-tx",
		.start_fn=_linkmod_files_thread_start,
		.stop_fn=_linkmod_files_thread_stop,
		.loop_fn=_linkmod_files_tx_thread_loop,
		.data=linkmod_files
	});
	return (PigeonLinkmod *)linkmod_files;
}

void linkmod_files_tx_free(PigeonLinkmod *linkmod) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)linkmod;
	long_thread_free(linkmod_files->public.long_thread);
	base64_free(linkmod_files->base64);
	free(linkmod_files);
}

bool linkmod_files_rx_is_available() {
	return getenv(FILES_DIR_RX_VAR_NAME) != NULL;
}

PigeonLinkmod *linkmod_files_rx_new() {
	LinkmodFiles *linkmod_files = malloc(sizeof(LinkmodFiles));
	memset(linkmod_files, 0, sizeof(*linkmod_files));
	linkmod_files->base64 = base64_new();
	linkmod_files->files_dir_path = getenv(FILES_DIR_RX_VAR_NAME);
	linkmod_files->public.long_thread = long_thread_new((LongThreadOptions){
		.name="linkmod-console-rx",
		.start_fn=_linkmod_files_thread_start,
		.stop_fn=_linkmod_files_thread_stop,
		.loop_fn=_linkmod_files_rx_thread_loop,
		.data=linkmod_files
	});
	return (PigeonLinkmod *)linkmod_files;
}

void linkmod_files_rx_free(PigeonLinkmod *linkmod) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)linkmod;
	long_thread_free(linkmod_files->public.long_thread);
	base64_free(linkmod_files->base64);
	free(linkmod_files);
}

bool _linkmod_files_thread_start(LongThread *long_thread, void *data) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)data;

	bool error = false;

	error = (linkmod_files->files_dir_path == NULL);

	if (!error) {
		linkmod_files->frames_file_path = path_join(
			linkmod_files->files_dir_path,
			strlen(linkmod_files->files_dir_path),
			FRAMES_TX_FILE_NAME,
			strlen(FRAMES_TX_FILE_NAME)
		);
		error = linkmod_files->frames_file_path == NULL;
	}

	return !error;
}

bool _linkmod_files_thread_stop(LongThread *long_thread, void *data) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)data;

	bool error = false;

	if (!error) {
		if (linkmod_files->frames_file_path) {
			free(linkmod_files->frames_file_path);
			linkmod_files->frames_file_path = NULL;
		}
	}

	return !error;
}

LongThreadResult _linkmod_files_tx_thread_loop(LongThread *long_thread, void *data) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)data;
	PigeonLink *pigeon_link = linkmod_files->public.pigeon_link;

	DIR *files_dir;

	files_dir = opendir(linkmod_files->files_dir_path);

	if (files_dir && pigeon_link_frames_wait(pigeon_link)) {
		// We can send files, and we have packets to send!
		_push_to_frames_file(linkmod_files, pigeon_link);
		closedir(files_dir);
	} else {
		nanosleep(&FILES_DIR_POLL_DELAY, NULL);
	}

	return LONG_THREAD_CONTINUE;
}

LongThreadResult _linkmod_files_rx_thread_loop(LongThread *long_thread, void *data) {
	LinkmodFiles *linkmod_files = (LinkmodFiles *)data;
	PigeonLink *pigeon_link = linkmod_files->public.pigeon_link;

	DIR *files_dir;

	files_dir = opendir(linkmod_files->files_dir_path);

	if (files_dir) {
		// There might be files available to read.
		_pop_from_frames_file(linkmod_files, pigeon_link);
		closedir(files_dir);
	}

	nanosleep(&FILES_DIR_POLL_DELAY, NULL);

	return LONG_THREAD_CONTINUE;
}

bool _push_to_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link) {
	FILE *frames_file = fopen(linkmod_files->frames_file_path, "a");

	// It is important that we open the file with O_SYNC so changes are committed to disk.
	int file_flags = fcntl(fileno(frames_file), F_GETFL) | O_DSYNC | O_RSYNC;
	fcntl(fileno(frames_file), F_SETFL, file_flags);

	if (frames_file) {
		while (pigeon_link_frames_has_next(pigeon_link)) {
			PigeonFrame *pigeon_frame = pigeon_link_frames_pop(pigeon_link);
			const unsigned char *raw_buffer;
			size_t raw_buffer_size;
			unsigned char *b64_buffer;
			size_t b64_buffer_size;

			raw_buffer_size = pigeon_frame_get_buffer(pigeon_frame, &raw_buffer);
			b64_buffer = base64_encode(linkmod_files->base64, raw_buffer, raw_buffer_size, &b64_buffer_size);

			if (b64_buffer != NULL) {
				fwrite(b64_buffer, sizeof(*b64_buffer), b64_buffer_size, frames_file);
				fprintf(frames_file, "\n");
				free(b64_buffer);
			}
		}

		fflush(frames_file);
		fclose(frames_file);
	} else {
		perror("Error opening frames file");
	}

	return true;
}

bool _pop_from_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link) {
	FILE *frames_file = fopen(linkmod_files->frames_file_path, "r+");

	// TODO: We should probably move frames_file to another location in case
	//       another program is writing to it for some reason.

	if (frames_file) {
		char raw_buffer[READ_BUFFER_SIZE];

		while (fgets(raw_buffer, sizeof(raw_buffer), frames_file)) {
			int line_length = strlen(raw_buffer);
			if (line_length > 0 && raw_buffer[line_length-1] == '\n') {
				// Remove \n from the end of each line. (It was added by us).
				line_length -= 1;

				unsigned char *decode_buffer;
				size_t decode_buffer_size;

				decode_buffer = base64_decode(linkmod_files->base64, (unsigned char *)raw_buffer, line_length, &decode_buffer_size);
				PigeonFrame *pigeon_frame = pigeon_frame_new(decode_buffer, decode_buffer_size);
				pigeon_link_frames_push(pigeon_link, pigeon_frame);
				free(decode_buffer);
			} else if (line_length > 0) {
				// This line is probably too long. That can't be good :(
				break;
			}
		}

		// Re-open the file to clear its contents
		frames_file = freopen(linkmod_files->frames_file_path, "w", frames_file);

		fclose(frames_file);
	}

	return true;
}