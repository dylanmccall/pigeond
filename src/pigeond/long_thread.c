#include "long_thread.h"

#include <pthread.h>
#include <string.h>
#include <stdio.h>

struct _LongThread {
	pthread_t thread;
	pthread_mutex_t thread_mutex;
	pthread_mutex_t thread_started_mutex;
	pthread_cond_t thread_started;
	volatile bool thread_stop;
	volatile bool finished;
	LongThreadOptions options;
};

LongThread *long_thread_new(LongThreadOptions options) {
	LongThread *long_thread = malloc(sizeof(LongThread));
	memset(long_thread, 0, sizeof(*long_thread));
	long_thread->thread = 0;
	long_thread->options = options;
	pthread_mutex_init(&long_thread->thread_mutex, NULL);
	pthread_mutex_init(&long_thread->thread_started_mutex, NULL);
	pthread_cond_init(&long_thread->thread_started, NULL);
	return long_thread;
}

void *_long_thread_thread_fn(void *arg);

void long_thread_free(LongThread *long_thread) {
	free(long_thread);
}

bool long_thread_start(LongThread *long_thread) {
	bool error = false;

	pthread_mutex_lock(&long_thread->thread_mutex);
	{
		if (long_thread->thread != 0) {
			error = true;
		}

		error = !error && !long_thread->options.start_fn(long_thread, long_thread->options.data);

		if (!error) {
			long_thread->thread_stop = false;
			if (pthread_create(&long_thread->thread, NULL, _long_thread_thread_fn, long_thread) != 0) {
				perror("Error creating thread");
				error = true;
			}
		}
	}
	pthread_mutex_unlock(&long_thread->thread_mutex);

	return !error;
}

bool long_thread_wait(LongThread *long_thread) {
	pthread_cond_wait(&long_thread->thread_started, &long_thread->thread_started_mutex);
	return true;
}

int long_thread_join(LongThread *long_thread) {
	return pthread_join(long_thread->thread, NULL);
}

bool long_thread_stop(LongThread *long_thread) {
	bool error = false;

	pthread_mutex_lock(&long_thread->thread_mutex);
	{
		if (long_thread->thread == 0) {
			error = true;
		}

		if (!error) {
			long_thread->thread_stop = true;
			if (pthread_join(long_thread->thread, NULL) != 0) {
				perror("Error exiting thread");
				error = true;
			}
		}

		error = !error && !long_thread->options.stop_fn(long_thread, long_thread->options.data);

		if (!error) {
			long_thread->thread = 0;
		}
	}
	pthread_mutex_unlock(&long_thread->thread_mutex);

	return !error;
}

bool long_thread_is_running(LongThread *long_thread) {
	return long_thread->thread != 0 && long_thread->finished == false;
}

void *_long_thread_thread_fn(void *arg) {
	LongThread *long_thread = (LongThread *)arg;

	long_thread_loop_t loop_fn = long_thread->options.loop_fn;

	pthread_cond_signal(&long_thread->thread_started);

	while (!long_thread->thread_stop) {
		LongThreadResult result = loop_fn(long_thread, long_thread->options.data);
		if (result == LONG_THREAD_STOP) {
			long_thread->thread_stop = true;
		}
	}

	long_thread->finished = true;

	return NULL;
}
