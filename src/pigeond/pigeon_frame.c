#include "pigeon_frame.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct _PigeonFrame {
	char *buffer;
	int buffer_size;
};

PigeonFrame *pigeon_frame_new(const char *buffer, size_t buffer_size) {
	PigeonFrame *pigeon_frame = malloc(sizeof(PigeonFrame));
	memset(pigeon_frame, 0, sizeof(*pigeon_frame));

	if (buffer_size < ETHER_MIN_LEN) {
		// Pad ethernet frames if needed
		pigeon_frame->buffer = malloc(ETHER_MIN_LEN);
		pigeon_frame->buffer_size = ETHER_MIN_LEN;
	} else {
		pigeon_frame->buffer = malloc(buffer_size);
		pigeon_frame->buffer_size = buffer_size;
	}

	memcpy(pigeon_frame->buffer, buffer, buffer_size);

	return pigeon_frame;
}

void pigeon_frame_free(PigeonFrame *pigeon_frame) {
	free(pigeon_frame->buffer);
	free(pigeon_frame);
}

size_t pigeon_frame_get_data(PigeonFrame *pigeon_frame, const char **out_data) {
	if (pigeon_frame->buffer_size > ETHER_HDR_LEN) {
		*out_data = pigeon_frame->buffer + ETHER_HDR_LEN;
		return pigeon_frame->buffer_size - ETHER_HDR_LEN;
	} else {
		return 0;
	}
}

void pigeon_frame_print_header(PigeonFrame *pigeon_frame) {
	struct ether_header *header;
	const char *data;
	size_t data_size;
	header = (struct ether_header *) pigeon_frame->buffer + 0;
	data_size = pigeon_frame_get_data(pigeon_frame, &data);

	printf("Destination: ");
	for (int i = 0; i < ETH_ALEN; i++) printf("%x ", header->ether_dhost[i]);
	printf("\n");

	printf("Source: ");
	for (int i = 0; i < ETH_ALEN; i++) printf("%x ", header->ether_shost[i]);
	printf("\n");

	printf("Type: %x\n", (int)header->ether_type);

	printf("Data: %d bytes\n", (int)data_size);
}

void pigeon_frame_print_data(PigeonFrame *pigeon_frame) {
	const char *data;
	size_t data_size = pigeon_frame_get_data(pigeon_frame, &data);

	if (data_size > 0) {
		bool in_alnum = false;
		for (int i = 0; i < data_size; i++) {
			char value = data[i];
			bool is_alnum = isalnum(value);

			if (is_alnum != in_alnum) {
				if (is_alnum) {
					printf("\"");
				} else {
					printf("\" ");
				}
			}

			if (is_alnum) {
				printf("%c", value);
			} else {
				printf("%x ", value);
			}

			in_alnum = is_alnum;
		}
		printf("\n");
	}
}
