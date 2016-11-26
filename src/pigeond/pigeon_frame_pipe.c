#include "pigeon_frame_pipe.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct _PigeonFramePipe {
	PigeonFrame *tx_buffer[PIGEON_FRAME_PIPE_SIZE];
	PigeonFrame *rx_buffer[PIGEON_FRAME_PIPE_SIZE];
	pthread_mutex_t tx_mutex;
	pthread_mutex_t rx_mutex;
};

PigeonFrame **_pigeon_frame_pipe_ref_get_write_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);
PigeonFrame **_pigeon_frame_pipe_ref_get_read_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);

PigeonFramePipe *pigeon_frame_pipe_new() {
	PigeonFramePipe *pigeon_frame_pipe = malloc(sizeof(PigeonFramePipe));
	memset(pigeon_frame_pipe, 0, sizeof(*pigeon_frame_pipe));
	pthread_mutex_init(&pigeon_frame_pipe->tx_mutex, NULL);
	pthread_mutex_init(&pigeon_frame_pipe->rx_mutex, NULL);
	return pigeon_frame_pipe;
}

void pigeon_frame_pipe_free(PigeonFramePipe *pigeon_frame_pipe) {
	free(pigeon_frame_pipe);
}

PigeonFramePipeHandle pigeon_frame_pipe_get_tx(PigeonFramePipe *pigeon_frame_pipe) {
	return (PigeonFramePipeHandle){
		.pigeon_frame_pipe=pigeon_frame_pipe,
		.direction=PIGEON_FRAME_PIPE_TX
	};
}

PigeonFramePipeHandle pigeon_frame_pipe_get_rx(PigeonFramePipe *pigeon_frame_pipe) {
	return (PigeonFramePipeHandle){
		.pigeon_frame_pipe=pigeon_frame_pipe,
		.direction=PIGEON_FRAME_PIPE_RX
	};
}

/**
 * Push a frame to the pipe. The caller loses ownership of the frame.
 * @return  true if the operation succeeds
 */
bool pigeon_frame_pipe_push(PigeonFramePipeHandle pigeon_frame_pipe_ref, PigeonFrame *pigeon_frame) {
	// PigeonFrame **write_buffer = _pigeon_frame_pipe_ref_get_write_buffer(pigeon_frame_pipe_ref);
	return false;
}

/**
 * Get the number of frames waiting to be read from the pipe.
 * @return  the number of frames
 */
size_t pigeon_frame_pipe_count(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	// PigeonFrame **read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);
	return 0;
}

/**
 * Pipe a frame from the pipe. If there are no frames, this operation will
 * block until one arrives. The caller gains ownership of the frame and must
 * call pigeon_frame_free when finished.
 * @return  the next PigeonFrame in the pipe, or NULL if there was an error
 */
PigeonFrame *pigeon_frame_pipe_pop(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	// PigeonFrame **read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);
	return NULL;
}

PigeonFrame **_pigeon_frame_pipe_ref_get_write_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFramePipe *pigeon_frame_pipe = pigeon_frame_pipe_ref.pigeon_frame_pipe;
	if (pigeon_frame_pipe_ref.direction == PIGEON_FRAME_PIPE_TX) {
		return pigeon_frame_pipe->tx_buffer;
	} else {
		return pigeon_frame_pipe->rx_buffer;
	}
}

PigeonFrame **_pigeon_frame_pipe_ref_get_read_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFramePipe *pigeon_frame_pipe = pigeon_frame_pipe_ref.pigeon_frame_pipe;
	if (pigeon_frame_pipe_ref.direction == PIGEON_FRAME_PIPE_TX) {
		return pigeon_frame_pipe->rx_buffer;
	} else {
		return pigeon_frame_pipe->tx_buffer;
	}
}
