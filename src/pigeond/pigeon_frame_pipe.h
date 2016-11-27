#ifndef _PIGEON_FRAME_PIPE_H
#define _PIGEON_FRAME_PIPE_H

/**
 * pigeon_frame_pipe.h
 * Pipe PigeonFrames between PigeonLink and PigeonTunnel. This provides a
 * two-way pipe, where both sides can both read and write.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "pigeon_frame.h"

#define PIGEON_FRAME_PIPE_SIZE 64

typedef struct _PigeonFramePipe PigeonFramePipe;

typedef struct _PigeonFramePipeHandle PigeonFramePipeHandle;

typedef enum {
    PIGEON_FRAME_PIPE_TX, // Write to TX, read from RX
    PIGEON_FRAME_PIPE_RX  // Write to RX, read from TX
} PigeonFramePipeDirection;

struct _PigeonFramePipeHandle {
	PigeonFramePipe *pigeon_frame_pipe;
	PigeonFramePipeDirection direction;
};

PigeonFramePipe *pigeon_frame_pipe_new();
void pigeon_frame_pipe_free(PigeonFramePipe *pigeon_frame_pipe);
PigeonFramePipeHandle pigeon_frame_pipe_get_tx(PigeonFramePipe *pigeon_frame_pipe);
PigeonFramePipeHandle pigeon_frame_pipe_get_rx(PigeonFramePipe *pigeon_frame_pipe);

/**
 * Push a frame to the write pipe. The caller loses ownership of the frame.
 * @return  true if the operation succeeds
 */
bool pigeon_frame_pipe_push(PigeonFramePipeHandle pigeon_frame_pipe_ref, PigeonFrame *pigeon_frame);

/**
 * Pipe a frame from the pipe. If there are no frames, this operation will
 * block until one arrives. The caller gains ownership of the frame and must
 * call pigeon_frame_free when finished.
 * @return  the next PigeonFrame in the read pipe, or NULL if there was an error
 */
PigeonFrame *pigeon_frame_pipe_pop(PigeonFramePipeHandle pigeon_frame_pipe_ref);

/**
 * Get the number of frames waiting to be read from the pipe.
 * @return  the number of frames in the read pipe
 */
size_t pigeon_frame_pipe_read_count(PigeonFramePipeHandle pigeon_frame_pipe_ref);

/**
 * Get whether there are no frames to be read from the pipe.
 * @return  true if there are no frames in the read pipe
 */
size_t pigeon_frame_pipe_read_is_empty(PigeonFramePipeHandle pigeon_frame_pipe_ref);

/**
 * Get whether the pipe is full. Additional frames sent to the pipe will be
 * dropped.
 * @return  true if the read pipe is full
 */
size_t pigeon_frame_pipe_read_is_full(PigeonFramePipeHandle pigeon_frame_pipe_ref);

#endif
