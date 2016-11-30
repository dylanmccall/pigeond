#include "pigeon_frame.h"

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <net/ethernet.h>

struct _PigeonFrame {
	unsigned char *buffer;
	size_t buffer_size;
	struct ether_header *header;
	const unsigned char *data;
	size_t data_size;
};

typedef struct {
	unsigned ether_type;
	const char *name;
} EtherTypeInfo;

const EtherTypeInfo ETHER_TYPE_INFO[] = {
	{ETHERTYPE_IP, "IP"},
	{ETHERTYPE_ARP, "ARP"},
	{ETHERTYPE_IPX, "IPX"},
	{ETHERTYPE_IPV6, "IPv6"},
	{ETHERTYPE_LOOPBACK, "LOOPBACK"}
};

const size_t ETHER_TYPE_INFO_COUNT = sizeof(ETHER_TYPE_INFO) / sizeof(*ETHER_TYPE_INFO);

const EtherTypeInfo *_pigeon_frame_get_ether_type_info(PigeonFrame *pigeon_frame);
size_t _find_string(const unsigned char *data, size_t offset, size_t data_size);
bool _eol_char(const unsigned char value);

PigeonFrame *pigeon_frame_new(const unsigned char *buffer, size_t buffer_size) {
	PigeonFrame *pigeon_frame = malloc(sizeof(PigeonFrame));
	memset(pigeon_frame, 0, sizeof(*pigeon_frame));

	bool error = false;

	if (!error) {
		if (buffer_size > ETHER_MAX_LEN) {
			// If the buffer is over-sized, we should stop immediately.
			fprintf(stderr, "Dropping over-sized frame\n");
			error = true;
		} else if (buffer_size < ETHER_MIN_LEN) {
			// Pad undersized frame with empty space
			buffer_size = ETHER_MIN_LEN;
		}
	}

	if (!error) {
		pigeon_frame->buffer = malloc(buffer_size);
		pigeon_frame->buffer_size = buffer_size;
		error = (pigeon_frame->buffer == NULL);
	}

	if (!error) {
		memcpy(pigeon_frame->buffer, buffer, buffer_size);

		pigeon_frame->header = (struct ether_header *) pigeon_frame->buffer + 0;

		if (pigeon_frame->buffer_size > ETHER_HDR_LEN) {
			pigeon_frame->data = pigeon_frame->buffer + ETHER_HDR_LEN;
			pigeon_frame->data_size = pigeon_frame->buffer_size - ETHER_HDR_LEN - 1;
		} else {
			pigeon_frame->data = NULL;
			pigeon_frame->data_size = 0;
		}
	}

	if (!error) {
		return pigeon_frame;
	} else {
		free(pigeon_frame);
		return NULL;
	}
}

void pigeon_frame_free(PigeonFrame *pigeon_frame) {
	free(pigeon_frame->buffer);
	free(pigeon_frame);
}

size_t pigeon_frame_get_buffer(PigeonFrame *pigeon_frame, const unsigned char **out_buffer) {
	*out_buffer = pigeon_frame->buffer;
	return pigeon_frame->buffer_size;
}

size_t pigeon_frame_get_data(PigeonFrame *pigeon_frame, const unsigned char **out_data) {
	*out_data = pigeon_frame->data;
	return pigeon_frame->data_size;
}

void pigeon_frame_print_header(PigeonFrame *pigeon_frame) {
	if (pigeon_frame->header) {
		const EtherTypeInfo *ether_type_info = _pigeon_frame_get_ether_type_info(pigeon_frame);

		printf("Destination: ");
		for (int i = 0; i < ETH_ALEN; i++) printf("%x ", pigeon_frame->header->ether_dhost[i]);
		printf("\n");

		printf("Source: ");
		for (int i = 0; i < ETH_ALEN; i++) printf("%x ", pigeon_frame->header->ether_shost[i]);
		printf("\n");

		if (ether_type_info != NULL) {
			printf("Type: %s\n", ether_type_info->name);
		} else {
			printf("Type: %x\n", ntohs(pigeon_frame->header->ether_type));
		}

		printf("Data: %lu bytes\n", (unsigned long)pigeon_frame->data_size);
	} else {
		printf("Empty frame\n");
	}
}

void pigeon_frame_print_data(PigeonFrame *pigeon_frame) {
	if (pigeon_frame->data) {
		size_t string_start = 0;
		size_t string_end = 0;

		for (size_t i = 0; i < pigeon_frame->data_size; i++) {
			unsigned char value = pigeon_frame->data[i];

			if (i > string_end) {
				string_end = _find_string(pigeon_frame->data, i, pigeon_frame->data_size);
				string_start = (string_end > 0) ? i : 0;
			}

			bool in_string = string_start != string_end && i >= string_start && i <= string_end;

			if (in_string) {
				if (i == string_start) printf("<");
				if (isprint(value)) {
					printf("%c", value);
				} else if (value == '\n') {
					printf(" ");
				}
				if (i == string_end) printf("> ");
			} else {
				printf("%x ", value);
			}
		}
		printf("\n");
	} else {
		printf("No data\n");
	}
}

const EtherTypeInfo *_pigeon_frame_get_ether_type_info(PigeonFrame *pigeon_frame) {
	unsigned ether_type = ntohs(pigeon_frame->header->ether_type);

	for (size_t i = 0; i < ETHER_TYPE_INFO_COUNT; i++) {
		const EtherTypeInfo *ether_type_info = &ETHER_TYPE_INFO[i];
		if (ether_type_info->ether_type == ether_type) {
			return ether_type_info;
		}
	}

	return NULL;
}

size_t _find_string(const unsigned char *data, size_t offset, size_t data_size) {
	assert(offset < data_size);
	size_t line_end = 0;
	for (size_t i = offset; i < data_size; i++) {
		unsigned char value = data[i];
		if (isprint(value)) {
			line_end = 0;
		} else if (i > offset+3 && _eol_char(value)) {
			// We mark a string ending if there have been more than three
			// printable characters followed by an end of line.
			line_end = i;
		} else if (line_end > 0) {
			break;
		} else {
			break;
		}
	}
	return line_end;
}

bool _eol_char(const unsigned char value) {
	return value == '\r' || value == '\n';
}
