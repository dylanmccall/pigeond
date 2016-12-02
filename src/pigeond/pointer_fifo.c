#include "pointer_fifo.h"

#include <string.h>

/**
 * FIXME: This Fifo implementation always leaves a blank space at maximum
 *        capacity, so it can only fit size-1 items.
 */

struct _PointerFifo {
	void **buffer;
	size_t head;
	size_t tail;
	size_t size;
};

PointerFifo *pointer_fifo_new(size_t size) {
	PointerFifo *fifo = malloc(sizeof(PointerFifo));
	memset(fifo, 0, sizeof(*fifo));
	fifo->buffer = calloc(size, sizeof(*fifo->buffer));
	fifo->size = size;
	return fifo;
}

void pointer_fifo_free(PointerFifo *fifo) {
	free(fifo->buffer);
	free(fifo);
}

int pointer_fifo_push(PointerFifo *fifo, void *data) {
	if (!pointer_fifo_is_full(fifo)) {
		fifo->buffer[fifo->head] = data;
		fifo->head = (fifo->head + 1) % fifo->size;
		return 1;
	} else {
		return 0;
	}
}

void *pointer_fifo_pop(PointerFifo *fifo) {
	if (!pointer_fifo_is_empty(fifo)) {
		void *result = fifo->buffer[fifo->tail];
		fifo->tail = (fifo->tail + 1) % fifo->size;
		return result;
	} else {
		return NULL;
	}
}

void pointer_fifo_clear(PointerFifo *fifo) {
	fifo->head = 0;
	fifo->tail = 0;
}

size_t pointer_fifo_count(PointerFifo *fifo) {
	size_t head = fifo->head;
	size_t tail = fifo->tail;
	
	if (head < tail) {
		return fifo->size - (tail - head);
	} else {
		return head - tail;
	}
}

bool pointer_fifo_is_empty(PointerFifo *fifo) {
	return pointer_fifo_count(fifo) == 0;
}

bool pointer_fifo_is_full(PointerFifo *fifo) {
	return pointer_fifo_count(fifo) == fifo->size - 1;
}
