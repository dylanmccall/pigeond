#ifndef _PIGEON_FRAME_H
#define _PIGEON_FRAME_H

/**
 * pigeon_frame.h
 * Structures and utilities for link-layer frames.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include <net/ethernet.h>

typedef struct _PigeonFrame PigeonFrame;

PigeonFrame *pigeon_frame_new(const char *buffer, size_t buffer_size);
void pigeon_frame_free(PigeonFrame *pigeon_frame);
size_t pigeon_frame_get_data(PigeonFrame *pigeon_frame, const char **out_data);
void pigeon_frame_print_header(PigeonFrame *pigeon_frame);
void pigeon_frame_print_data(PigeonFrame *pigeon_frame);

#endif
