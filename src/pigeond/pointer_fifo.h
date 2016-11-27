#ifndef _POINTER_FIFO_H
#define _POINTER_FIFO_H

/**
 * pointer_fifo.h
 * Simple fixed-size Fifo buffer that holds arbitrary pointers.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _PointerFifo PointerFifo;

PointerFifo *pointer_fifo_new(size_t size);
void pointer_fifo_free(PointerFifo *pointer_fifo);
int pointer_fifo_push(PointerFifo *pointer_fifo, void *data);
void *pointer_fifo_pop(PointerFifo *pointer_fifo);
size_t pointer_fifo_count(PointerFifo *pointer_fifo);
bool pointer_fifo_is_empty(PointerFifo *pointer_fifo);
bool pointer_fifo_is_full(PointerFifo *pointer_fifo);

#endif
