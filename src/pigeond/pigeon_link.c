#include "pigeon_link.h"

#include "pigeon_linkmod.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct _PigeonLink {
	PigeonFrame *tx_buffer[PIGEON_RX_BUFFER_SIZE];
	PigeonFrame *rx_buffer[PIGEON_RX_BUFFER_SIZE];
	PigeonLinkmod *linkmod_tx;
	PigeonLinkmod *linkmod_rx;
	pthread_mutex_t tx_mutex;
	pthread_mutex_t rx_mutex;
};

void *_pigeon_link_thread(void *arg);

PigeonLink *pigeon_link_new() {
	PigeonLink *pigeon_link = malloc(sizeof(PigeonLink));
	memset(pigeon_link, 0, sizeof(*pigeon_link));
	pthread_mutex_init(&pigeon_link->tx_mutex, NULL);
	pthread_mutex_init(&pigeon_link->rx_mutex, NULL);
	return pigeon_link;
}

void pigeon_link_free(PigeonLink *pigeon_link) {
	if (pigeon_link->linkmod_tx) {
		pigeon_linkmod_free(pigeon_link->linkmod_tx);
	}
	if (pigeon_link->linkmod_rx) {
		pigeon_linkmod_free(pigeon_link->linkmod_rx);
	}
	free(pigeon_link);
}

bool pigeon_link_init(PigeonLink *pigeon_link) {
	bool error = false;

	if (!error) {
		pigeon_link->linkmod_tx = pigeon_linkmod_new_tx(pigeon_link);
		pigeon_link->linkmod_rx = pigeon_linkmod_new_rx(pigeon_link);
		error = pigeon_link->linkmod_tx == NULL || pigeon_link->linkmod_rx == NULL;
	}

	return !error;
}

bool pigeon_link_start(PigeonLink *pigeon_link) {
	bool error = false;

	if (!error) {
		error = pigeon_link->linkmod_tx == NULL || pigeon_link->linkmod_rx == NULL;
	}

	if (!error) {
		bool tx_started = pigeon_linkmod_start(pigeon_link->linkmod_tx);
		bool rx_started = pigeon_linkmod_start(pigeon_link->linkmod_rx);
		error = !tx_started || !rx_started;
	}

	return !error;
}

bool pigeon_link_wait(PigeonLink *pigeon_link) {
	bool tx_success, rx_success;

	if (pigeon_link->linkmod_tx) {
		tx_success = pigeon_linkmod_wait(pigeon_link->linkmod_tx);
	} else {
		tx_success = true;
	}

	if (pigeon_link->linkmod_rx) {
		rx_success = pigeon_linkmod_wait(pigeon_link->linkmod_rx);
	} else {
		rx_success = true;
	}

	return tx_success && rx_success;
}

int pigeon_link_join(PigeonLink *pigeon_link) {
	int tx_result, rx_result;

	if (pigeon_link->linkmod_tx) {
		tx_result = pigeon_linkmod_join(pigeon_link->linkmod_tx);
	} else {
		tx_result = 0;
	}

	if (pigeon_link->linkmod_rx) {
		rx_result = pigeon_linkmod_join(pigeon_link->linkmod_rx);
	} else {
		rx_result = 0;
	}

	return (tx_result == 0 && rx_result == 0) ? 0 : -1;
}

bool pigeon_link_stop(PigeonLink *pigeon_link) {
	bool tx_success, rx_success;

	if (pigeon_link->linkmod_tx) {
		tx_success = pigeon_linkmod_stop(pigeon_link->linkmod_tx);
	} else {
		tx_success = true;
	}

	if (pigeon_link->linkmod_rx) {
		rx_success = pigeon_linkmod_stop(pigeon_link->linkmod_rx);
	} else {
		rx_success = true;
	}

	return tx_success && rx_success;
}

bool pigeon_link_is_running(PigeonLink *pigeon_link) {
	bool tx_running, rx_running;

	if (pigeon_link->linkmod_tx) {
		tx_running = pigeon_linkmod_is_running(pigeon_link->linkmod_tx);
	} else {
		tx_running = false;
	}

	if (pigeon_link->linkmod_rx) {
		rx_running = pigeon_linkmod_is_running(pigeon_link->linkmod_rx);
	} else {
		rx_running = false;
	}

	return tx_running || rx_running;
}

void pigeon_link_print_debug_info(PigeonLink *pigeon_link) {
	const char *tx_name;
	const char *rx_name;

	if (pigeon_link->linkmod_tx) {
		tx_name = pigeon_linkmod_get_name(pigeon_link->linkmod_tx);
	} else {
		tx_name = "[none]";
	}

	if (pigeon_link->linkmod_rx) {
		rx_name = pigeon_linkmod_get_name(pigeon_link->linkmod_rx);
	} else {
		rx_name = "[none]";
	}

	printf("TX link module: %s\n", tx_name);
	printf("RX link module: %s\n", rx_name);
}

bool pigeon_link_tx_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame) {
	// Add frame to fifo buffer or pipe. Non-blocking.
	// Called by PigeonTunnel, on read.
	return false;
}

size_t pigeon_link_tx_count(PigeonLink *pigeon_link) {
	// Count frames in fifo buffer or pipe.
	return 0;
}

PigeonFrame *pigeon_link_tx_pop(PigeonLink *pigeon_link) {
	// Pop the next frame from fifo buffer or pipe. Blocking.
	// Called by Linkmod (TX), on send.
	return NULL;
}

bool pigeon_link_rx_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame) {
	// Add frame to fifo bffer or pipe. Non-blocking.
	// Called by Linkmod (RX), on read.
	return false;
}

size_t pigeon_link_rx_count(PigeonLink *pigeon_link) {
	// Count frames in fifo buffer or pipe.
	return 0;
}

PigeonFrame *pigeon_link_rx_pop(PigeonLink *pigeon_link) {
	// Pop the next frame from fifo buffer or pipe. Blocking.
	// Called by PigeonTunnel, on write.
	return NULL;
}
