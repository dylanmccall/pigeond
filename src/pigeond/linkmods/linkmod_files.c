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
#include "../audioMixer.h"

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

#define MAX_WRITE_COUNT 1000
#define MAX_WRITE_TIME_MS 10 * SECONDS_IN_MILLISECONDS

#define READ_BUFFER_SIZE PIGEON_LINK_MTU * 10

static struct timespec FILES_DIR_POLL_DELAY = {
	.tv_sec=1,
	.tv_nsec=0
};

static struct timespec FRAMES_POLL_DELAY = {
	.tv_sec=0,
	.tv_nsec=50 * MILLISECONDS_IN_NANOSECONDS
};

typedef struct {
	PigeonLinkmod public;
	const char *files_dir_path;
	char *frames_file_path;
	Base64 *base64;
	bool transfer_complete;
	wavedata_t *beep;
} LinkmodFiles;

bool _linkmod_files_thread_start(LongThread *long_thread, void *data);
bool _linkmod_files_thread_stop(LongThread *long_thread, void *data);

LongThreadResult _linkmod_files_tx_thread_loop(LongThread *long_thread, void *data);
bool _push_to_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link);

LongThreadResult _linkmod_files_rx_thread_loop(LongThread *long_thread, void *data);
bool _pop_from_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link);

unsigned char *_pigeon_frame_to_b64(Base64 *base64, PigeonFrame *pigeon_frame, size_t *out_b64_buffer_size);
bool _pigeon_frame_to_file(Base64 *base64, PigeonFrame *pigeon_frame, FILE *file);
PigeonFrame *_b64_to_pigeon_frame(Base64 *base64, const unsigned char *b64_buffer, size_t b64_buffer_size);

bool linkmod_files_tx_is_available() {
	return getenv(FILES_DIR_TX_VAR_NAME) != NULL;
}

PigeonLinkmod *linkmod_files_tx_new() {
	LinkmodFiles *linkmod_files = malloc(sizeof(LinkmodFiles));
	memset(linkmod_files, 0, sizeof(*linkmod_files));
	linkmod_files->base64 = base64_new();
	linkmod_files->files_dir_path = getenv(FILES_DIR_TX_VAR_NAME);
	linkmod_files->beep = malloc(sizeof(wavedata_t));
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
	AudioMixer_freeWaveFileData(linkmod_files->beep);
	free(linkmod_files->beep);
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
		AudioMixer_readWaveFileIntoMemory("data/SoundEffects/beep.wav", linkmod_files->beep);
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

	if (files_dir && !linkmod_files->transfer_complete) {
		// There might be files available to read.
		fprintf(stderr, "Device connected. Writing frames...\n");
		_push_to_frames_file(linkmod_files, pigeon_link);
		closedir(files_dir);
	} else if (!files_dir && linkmod_files->transfer_complete) {
		// Once the device is removed, we should be ready to transfer files again
		fprintf(stderr, "Device removed.\n");
		linkmod_files->transfer_complete = false;
	}

	nanosleep(&FILES_DIR_POLL_DELAY, NULL);

	return LONG_THREAD_CONTINUE;
}

bool _push_to_frames_file(LinkmodFiles *linkmod_files, PigeonLink *pigeon_link) {
	// It is important that we open the file with O_SYNC so changes are committed to disk.
	FILE *frames_file = fopen(linkmod_files->frames_file_path, "a");

	if (frames_file == NULL) {
		perror("Error opening frames file");
		return false;
	} else {
		int file_flags = fcntl(fileno(frames_file), F_GETFL) | O_DSYNC | O_RSYNC;
		fcntl(fileno(frames_file), F_SETFL, file_flags);
	}

	struct timespec transfer_start_time = {0, 0};
	struct timespec now_time = {0, 0};
	int frames_count = 0;
	bool error = false;

	while (!error && !linkmod_files->transfer_complete) {
		PigeonFrame *pigeon_frame;

		if (pigeon_link_frames_has_next(pigeon_link)) {
			pigeon_frame = pigeon_link_frames_pop(pigeon_link);
		} else {
			pigeon_frame = NULL;
		}

		clock_gettime(CLOCK_MONOTONIC, &now_time);

		if (pigeon_frame) {
			if (_pigeon_frame_to_file(linkmod_files->base64, pigeon_frame, frames_file)) {
				// Frame was written successfully. Yay! Count this one.
				fprintf(stderr, "Wrote frame to file\n");
				if (frames_count == 0) {
					// Start counting to MAX_WRITE_COUNT from the first frame
					transfer_start_time = now_time;
				}
				frames_count += 1;
			} else {
				// If anything goes wrong, exit.
				fprintf(stderr, "Something went wrong :(\n");
				error = true;
			}
		}

		if (frames_count >= MAX_WRITE_COUNT) {
			linkmod_files->transfer_complete = true;
		} else if (frames_count > 0 && timespec_delta_milliseconds(&transfer_start_time, &now_time) >= MAX_WRITE_TIME_MS) {
			linkmod_files->transfer_complete = true;
		}

		if (!error && !linkmod_files->transfer_complete) {
			nanosleep(&FRAMES_POLL_DELAY, NULL);
		}
	}

	if (frames_file) {
		fflush(frames_file);
		fclose(frames_file);
	}

	if (linkmod_files->transfer_complete) {
		fprintf(stderr, "Finished writing frames to file\n");
		AudioMixer_queueSound(linkmod_files->beep);
	} else if (error) {
		fprintf(stderr, "Cancelled writing frames to file\n");
		linkmod_files->transfer_complete = true;
		AudioMixer_queueSound(linkmod_files->beep);
		// TODO: And another beep sound.
	}

	return !error;
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
				PigeonFrame *pigeon_frame = _b64_to_pigeon_frame(linkmod_files->base64, (unsigned char *)raw_buffer, line_length);
				if (pigeon_frame != NULL) {
					pigeon_link_frames_push(pigeon_link, pigeon_frame);
				}
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

unsigned char *_pigeon_frame_to_b64(Base64 *base64, PigeonFrame *pigeon_frame, size_t *out_b64_buffer_size) {
	const unsigned char *raw_buffer;
	size_t raw_buffer_size;
	raw_buffer_size = pigeon_frame_get_buffer(pigeon_frame, &raw_buffer);
	return base64_encode(base64, raw_buffer, raw_buffer_size, out_b64_buffer_size);
}

bool _pigeon_frame_to_file(Base64 *base64, PigeonFrame *pigeon_frame, FILE *frames_file) {	
	size_t b64_buffer_size;
	unsigned char *b64_buffer = _pigeon_frame_to_b64(base64, pigeon_frame, &b64_buffer_size);
	const char *newline = "\n";

	if (b64_buffer != NULL) {
		bool error = false;

		if (!error) {
			error = fwrite(b64_buffer, sizeof(*b64_buffer), b64_buffer_size, frames_file) == 0;
		}

		if (!error) {
			error = fwrite(newline, sizeof(*newline), 1, frames_file) == 0;
		}

		if (error) {
			perror("Error writing frame to file");
		}

		free(b64_buffer);

		return !error;
	} else {
		return false;
	}
}

PigeonFrame *_b64_to_pigeon_frame(Base64 *base64, const unsigned char *b64_buffer, size_t b64_buffer_size) {
	unsigned char *decode_buffer;
	size_t decode_buffer_size;
	decode_buffer = base64_decode(base64, b64_buffer, b64_buffer_size, &decode_buffer_size);
	PigeonFrame *pigeon_frame = pigeon_frame_new(decode_buffer, decode_buffer_size);
	free(decode_buffer);
	return pigeon_frame;
}
