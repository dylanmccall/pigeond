#include "pigeon_transmit.h"

#include "util.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct _PigeonTransmit {
	pthread_t thread;
	pthread_mutex_t thread_mutex;
	pthread_mutex_t thread_started_mutex;
	pthread_cond_t thread_started;
	volatile bool thread_stop;
	volatile bool finished;
};

void *_pigeon_transmit_thread(void *arg);

PigeonTransmit *pigeon_transmit_new() {
	PigeonTransmit *pigeon_transmit = malloc(sizeof(PigeonTransmit));
	memset(pigeon_transmit, 0, sizeof(*pigeon_transmit));
	pigeon_transmit->thread = 0;
	pthread_mutex_init(&pigeon_transmit->thread_mutex, NULL);
	pthread_mutex_init(&pigeon_transmit->thread_started_mutex, NULL);
	pthread_cond_init(&pigeon_transmit->thread_started, NULL);
	return pigeon_transmit;
}

void pigeon_transmit_free(PigeonTransmit *pigeon_transmit) {
	free(pigeon_transmit);
}

bool pigeon_transmit_start(PigeonTransmit *pigeon_transmit) {
	bool error = false;

	pthread_mutex_lock(&pigeon_transmit->thread_mutex);
	{
		if (pigeon_transmit->thread != 0) {
			error = true;
		}

		if (!error) {
			pigeon_transmit->thread_stop = false;
			if (pthread_create(&pigeon_transmit->thread, NULL, _pigeon_transmit_thread, pigeon_transmit) != 0) {
				perror("Error creating pigeon transmit thread");
				error = true;
			}
		}
	}
	pthread_mutex_unlock(&pigeon_transmit->thread_mutex);

	return !error;
}

bool pigeon_transmit_wait(PigeonTransmit *pigeon_transmit) {
	pthread_cond_wait(&pigeon_transmit->thread_started, &pigeon_transmit->thread_started_mutex);
	return true;
}

int pigeon_transmit_join(PigeonTransmit *pigeon_transmit) {
	return pthread_join(pigeon_transmit->thread, NULL);
}

bool pigeon_transmit_stop(PigeonTransmit *pigeon_transmit) {
	bool error = false;

	pthread_mutex_lock(&pigeon_transmit->thread_mutex);
	{
		if (pigeon_transmit->thread == 0) {
			error = true;
		}

		if (!error) {
			pigeon_transmit->thread_stop = true;
			if (pthread_join(pigeon_transmit->thread, NULL) != 0) {
				perror("Error exiting pigeon transmit thread");
				error = true;
			}
		}

		if (!error) {
			pigeon_transmit->thread = 0;
		}
	}
	pthread_mutex_unlock(&pigeon_transmit->thread_mutex);

	return !error;
}

bool pigeon_transmit_is_running(PigeonTransmit *pigeon_transmit) {
	return pigeon_transmit->thread != 0 && pigeon_transmit->finished == false;
}

void *_pigeon_transmit_thread(void *arg) {
	PigeonTransmit *pigeon_transmit = (PigeonTransmit *)arg;

	const struct timespec poll_delay = {
		.tv_sec=0,
		.tv_nsec=10 * MILLISECONDS_IN_NANOSECONDS
	};

	pthread_cond_signal(&pigeon_transmit->thread_started);

	while (!pigeon_transmit->thread_stop) {
		// TODO: Poll hardware devices for send and receive signals.
		// TODO: Read from barcode scanner and push to receive buffer.
		// TODO: Pop from send buffer and send to printer.
		nanosleep(&poll_delay, NULL);
	}

	pigeon_transmit->finished = true;

	return NULL;
}
