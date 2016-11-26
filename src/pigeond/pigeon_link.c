#include "pigeon_link.h"

#include "pigeon_linkmod.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

struct _PigeonLink {
	PigeonFramePipeHandle frame_pipe_ref_rx;
	PigeonLinkmod *linkmod_tx;
	PigeonLinkmod *linkmod_rx;
};

void *_pigeon_link_thread(void *arg);

PigeonLink *pigeon_link_new(PigeonFramePipeHandle frame_pipe_ref_rx) {
	PigeonLink *pigeon_link = malloc(sizeof(PigeonLink));
	pigeon_link->frame_pipe_ref_rx = frame_pipe_ref_rx;
	memset(pigeon_link, 0, sizeof(*pigeon_link));
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
	bool tx_error, rx_error;

	if (pigeon_link->linkmod_tx) {
		tx_error = pigeon_linkmod_wait(pigeon_link->linkmod_tx);
	} else {
		tx_error = false;
	}

	if (pigeon_link->linkmod_rx) {
		rx_error = pigeon_linkmod_wait(pigeon_link->linkmod_rx);
	} else {
		rx_error = false;
	}

	return !tx_error && !rx_error;
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
	bool tx_error, rx_error;

	if (pigeon_link->linkmod_tx) {
		tx_error = pigeon_linkmod_stop(pigeon_link->linkmod_tx);
	} else {
		tx_error = false;
	}

	if (pigeon_link->linkmod_rx) {
		rx_error = pigeon_linkmod_stop(pigeon_link->linkmod_rx);
	} else {
		rx_error = false;
	}

	return !tx_error && !rx_error;
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

PigeonFrame *pigeon_link_frames_pop(PigeonLink *pigeon_link) {
	return pigeon_frame_pipe_pop(pigeon_link->frame_pipe_ref_rx);
}

bool pigeon_link_frames_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame) {
	return pigeon_frame_pipe_push(pigeon_link->frame_pipe_ref_rx, pigeon_frame);
}

size_t pigeon_link_frames_count(PigeonLink *pigeon_link) {
	return pigeon_frame_pipe_count(pigeon_link->frame_pipe_ref_rx);
}
