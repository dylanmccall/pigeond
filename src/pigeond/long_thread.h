#ifndef _LONG_THREAD_H
#define _LONG_THREAD_H

/**
 * long_thread.h
 * Manages a long-running background thread.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _LongThread LongThread;

typedef struct _LongThreadOptions LongThreadOptions;

typedef enum {
    LONG_THREAD_CONTINUE = 0,
    LONG_THREAD_STOP
} LongThreadResult;

typedef bool (*long_thread_new_t)(LongThread *, void *);
typedef bool (*long_thread_start_t)(LongThread *, void *);
typedef bool (*long_thread_stop_t)(LongThread *, void *);
typedef bool (*long_thread_free_t)(LongThread *, void *);
typedef LongThreadResult (*long_thread_loop_t)(LongThread *, void *);

struct _LongThreadOptions {
	const char *name;
	long_thread_new_t new_fn; // Global initialization
	long_thread_start_t start_fn; // Setup before thread starts
	long_thread_stop_t stop_fn; // Teardown after thread ends
	long_thread_free_t free_fn; // Free resources from new_fn
	long_thread_loop_t loop_fn; // Thread main loop callback
	void *data;
};

LongThread *long_thread_new(LongThreadOptions options);
void long_thread_free(LongThread *long_thread);
bool long_thread_start(LongThread *long_thread);
bool long_thread_wait(LongThread *long_thread);
int long_thread_join(LongThread *long_thread);
bool long_thread_stop(LongThread *long_thread);
bool long_thread_teardown(LongThread *long_thread);
bool long_thread_is_running(LongThread *long_thread);

#endif