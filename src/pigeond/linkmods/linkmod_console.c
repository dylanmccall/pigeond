#include "linkmod_console.h"

#include <stdio.h>

LongThreadResult _linkmod_console_tx_thread_loop(LongThread *long_thread, void *data);
LongThreadResult _linkmod_console_rx_thread_loop(LongThread *long_thread, void *data);

bool linkmod_console_tx_is_available() {
	return true;
}

LongThread *linkmod_console_tx_new_thread(PigeonLink *pigeon_link) {
	return long_thread_new((LongThreadOptions){
		.name="linkmod-echo-tx",
		.loop_fn=_linkmod_console_tx_thread_loop,
		.data=pigeon_link
	});
}

bool linkmod_console_rx_is_available() {
	return true;
}

LongThread *linkmod_console_rx_new_thread(PigeonLink *pigeon_link) {
	return long_thread_new((LongThreadOptions){
		.name="linkmod-echo-rx",
		.loop_fn=_linkmod_console_rx_thread_loop,
		.data=pigeon_link
	});
}

LongThreadResult _linkmod_console_tx_thread_loop(LongThread *long_thread, void *data) {
	// PigeonLink *pigeon_link = (PigeonLink *)data;

	// Pop packet.
	// Echo packet.
	// Sleep?

	return LONG_THREAD_CONTINUE;
}

LongThreadResult _linkmod_console_rx_thread_loop(LongThread *long_thread, void *data) {
	// PigeonLink *pigeon_link = (PigeonLink *)data;

	// Read packet.
	// Push packet.

	return LONG_THREAD_CONTINUE;
}
