#include "pigeon_frame_pipe.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

typedef struct _PipeBuffer PipeBuffer;

struct _PipeBuffer {
	PigeonFrame *buffer[PIGEON_FRAME_PIPE_SIZE];
	size_t start_index;
	size_t end_index;
	pthread_mutex_t buffer_mutex;
	pthread_mutex_t write_cond_mutex;
	pthread_cond_t write_cond;
};

struct _PigeonFramePipe {
	PipeBuffer tx;
	PipeBuffer rx;
};

void _pigeon_frame_pipe_buffer_init(PipeBuffer *pipe_buffer);
PipeBuffer *_pigeon_frame_pipe_ref_get_write_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);
PipeBuffer *_pigeon_frame_pipe_ref_get_read_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);

PigeonFramePipe *pigeon_frame_pipe_new() {
	PigeonFramePipe *pigeon_frame_pipe = malloc(sizeof(PigeonFramePipe));
	memset(pigeon_frame_pipe, 0, sizeof(*pigeon_frame_pipe));
	_pigeon_frame_pipe_buffer_init(&pigeon_frame_pipe->tx);
	_pigeon_frame_pipe_buffer_init(&pigeon_frame_pipe->rx);
	return pigeon_frame_pipe;
}

void _pigeon_frame_pipe_buffer_init(PipeBuffer *pipe_buffer) {
	pthread_mutex_init(&pipe_buffer->buffer_mutex, NULL);
	pthread_mutex_init(&pipe_buffer->write_cond_mutex, NULL);
	pthread_cond_init(&pipe_buffer->write_cond, NULL);
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
	bool success;
	PipeBuffer *write_buffer = _pigeon_frame_pipe_ref_get_write_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&write_buffer->buffer_mutex);
	{
		success = true;
	}
	pthread_mutex_unlock(&write_buffer->buffer_mutex);

	if (success) {
		printf("PUSHED FRAME\n");
		pthread_cond_signal(&write_buffer->write_cond);
	}

	return success;
}

/**
 * Get the number of frames waiting to be read from the pipe.
 * @return  the number of frames
 */
size_t pigeon_frame_pipe_count(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	size_t count;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&read_buffer->buffer_mutex);
	{
		count = 0;
	}
	pthread_mutex_unlock(&read_buffer->buffer_mutex);

	return count;
}

/**
 * Pipe a frame from the pipe. If there are no frames, this operation will
 * block until one arrives. The caller gains ownership of the frame and must
 * call pigeon_frame_free when finished.
 * @return  the next PigeonFrame in the pipe, or NULL if there was an error
 */
PigeonFrame *pigeon_frame_pipe_pop(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFrame *result;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	pthread_cond_wait(&read_buffer->write_cond, &read_buffer->write_cond_mutex);

	pthread_mutex_lock(&read_buffer->buffer_mutex);
	{
		result = NULL;
	}
	pthread_mutex_unlock(&read_buffer->buffer_mutex);

	return result;
}

PipeBuffer *_pigeon_frame_pipe_ref_get_write_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFramePipe *pigeon_frame_pipe = pigeon_frame_pipe_ref.pigeon_frame_pipe;
	if (pigeon_frame_pipe_ref.direction == PIGEON_FRAME_PIPE_TX) {
		return &pigeon_frame_pipe->tx;
	} else {
		return &pigeon_frame_pipe->rx;
	}
}

PipeBuffer *_pigeon_frame_pipe_ref_get_read_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFramePipe *pigeon_frame_pipe = pigeon_frame_pipe_ref.pigeon_frame_pipe;
	if (pigeon_frame_pipe_ref.direction == PIGEON_FRAME_PIPE_TX) {
		return &pigeon_frame_pipe->rx;
	} else {
		return &pigeon_frame_pipe->tx;
	}
}
