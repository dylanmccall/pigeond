#include "pigeon_frame_pipe.h"

#include "pointer_fifo.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

typedef struct _PipeBuffer PipeBuffer;

struct _PipeBuffer {
	PointerFifo *fifo;
	pthread_mutex_t fifo_mutex;
	pthread_mutex_t write_cond_mutex;
	pthread_cond_t write_cond;
};

struct _PigeonFramePipe {
	PipeBuffer tx;
	PipeBuffer rx;
};

PipeBuffer *_pigeon_frame_pipe_ref_get_write_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);
PipeBuffer *_pigeon_frame_pipe_ref_get_read_buffer(PigeonFramePipeHandle pigeon_frame_pipe_ref);

PigeonFramePipe *pigeon_frame_pipe_new() {
	PigeonFramePipe *pigeon_frame_pipe = malloc(sizeof(PigeonFramePipe));
	memset(pigeon_frame_pipe, 0, sizeof(*pigeon_frame_pipe));

	pigeon_frame_pipe->tx.fifo = pointer_fifo_new(PIGEON_FRAME_PIPE_SIZE);
	pthread_mutex_init(&pigeon_frame_pipe->tx.fifo_mutex, NULL);
	pthread_mutex_init(&pigeon_frame_pipe->tx.write_cond_mutex, NULL);
	pthread_cond_init(&pigeon_frame_pipe->tx.write_cond, NULL);

	pigeon_frame_pipe->rx.fifo = pointer_fifo_new(PIGEON_FRAME_PIPE_SIZE);
	pthread_mutex_init(&pigeon_frame_pipe->rx.fifo_mutex, NULL);
	pthread_mutex_init(&pigeon_frame_pipe->rx.write_cond_mutex, NULL);
	pthread_cond_init(&pigeon_frame_pipe->rx.write_cond, NULL);

	return pigeon_frame_pipe;
}

void pigeon_frame_pipe_free(PigeonFramePipe *pigeon_frame_pipe) {
	pointer_fifo_free(pigeon_frame_pipe->tx.fifo);
	pointer_fifo_free(pigeon_frame_pipe->rx.fifo);
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

bool pigeon_frame_pipe_push(PigeonFramePipeHandle pigeon_frame_pipe_ref, PigeonFrame *pigeon_frame) {
	bool success;
	PipeBuffer *write_buffer = _pigeon_frame_pipe_ref_get_write_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&write_buffer->fifo_mutex);
	{
		success = pointer_fifo_push(write_buffer->fifo, (void *)pigeon_frame);
	}
	pthread_mutex_unlock(&write_buffer->fifo_mutex);

	if (success) {
		printf("PUSHED FRAME\n");
		pthread_cond_signal(&write_buffer->write_cond);
	}

	return success;
}

size_t pigeon_frame_pipe_count(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	size_t count;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&read_buffer->fifo_mutex);
	{
		count = pointer_fifo_count(read_buffer->fifo);
	}
	pthread_mutex_unlock(&read_buffer->fifo_mutex);

	return count;
}

size_t pigeon_frame_pipe_is_empty(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	bool result;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&read_buffer->fifo_mutex);
	{
		result = pointer_fifo_is_empty(read_buffer->fifo);
	}
	pthread_mutex_unlock(&read_buffer->fifo_mutex);

	return result;
}

size_t pigeon_frame_pipe_is_full(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	bool result;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	pthread_mutex_lock(&read_buffer->fifo_mutex);
	{
		result = pointer_fifo_is_full(read_buffer->fifo);
	}
	pthread_mutex_unlock(&read_buffer->fifo_mutex);

	return result;
}

PigeonFrame *pigeon_frame_pipe_pop(PigeonFramePipeHandle pigeon_frame_pipe_ref) {
	PigeonFrame *result;
	PipeBuffer *read_buffer = _pigeon_frame_pipe_ref_get_read_buffer(pigeon_frame_pipe_ref);

	while (pigeon_frame_pipe_is_empty(pigeon_frame_pipe_ref)) {
		pthread_cond_wait(&read_buffer->write_cond, &read_buffer->write_cond_mutex);
	}

	pthread_mutex_lock(&read_buffer->fifo_mutex);
	{
		result = (PigeonFrame *)pointer_fifo_pop(read_buffer->fifo);
	}
	pthread_mutex_unlock(&read_buffer->fifo_mutex);

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
