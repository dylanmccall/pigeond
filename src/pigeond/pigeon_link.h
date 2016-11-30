#ifndef _PIGEON_LINK_H
#define _PIGEON_LINK_H

/**
 * pigeon_link.h
 * Asynchronously send or receive ethernet frames over the physical layer.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "pigeon_frame.h"
#include "pigeon_frame_pipe.h"

#define PIGEON_LINK_MTU 170

typedef struct _PigeonLink PigeonLink;

PigeonLink *pigeon_link_new(PigeonFramePipeHandle frame_pipe_ref_rx);
void pigeon_link_free(PigeonLink *pigeon_link);
bool pigeon_link_init(PigeonLink *pigeon_link);
bool pigeon_link_start(PigeonLink *pigeon_link);
bool pigeon_link_wait(PigeonLink *pigeon_link);
int pigeon_link_join(PigeonLink *pigeon_link);
bool pigeon_link_stop(PigeonLink *pigeon_link);
bool pigeon_link_is_running(PigeonLink *pigeon_link);

void pigeon_link_print_debug_info(PigeonLink *pigeon_link);

bool pigeon_link_frames_wait(PigeonLink *pigeon_link);
bool pigeon_link_frames_has_next(PigeonLink *pigeon_link);
PigeonFrame *pigeon_link_frames_pop(PigeonLink *pigeon_link);
bool pigeon_link_frames_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame);

#endif
