#include "linkmod_null.h"

#include <stdio.h>
#include <string.h>

/**
 * Data used in this module, as well as public data that is shared with the
 * rest of the application. Every linkmod should create a struct with a
 * PigeonLinkmod as its first parameter.
 */
typedef struct {
	PigeonLinkmod public;
} LinkmodNull;

bool linkmod_null_tx_is_available() {
	return true;
}

PigeonLinkmod *linkmod_null_tx_new() {
	LinkmodNull *linkmod_null = malloc(sizeof(LinkmodNull));
	memset(linkmod_null, 0, sizeof(*linkmod_null));
	linkmod_null->public.long_thread = long_thread_new((LongThreadOptions){
		.name="linkmod-null-tx",
		.data=linkmod_null
	});
	return (PigeonLinkmod *)linkmod_null;
}

/**
 * Free all the data we created in our constructor, including the LongThread
 * that we created.
 */
void linkmod_null_tx_free(PigeonLinkmod *linkmod) {
	LinkmodNull *linkmod_console = (LinkmodNull *)linkmod;
	long_thread_free(linkmod_console->public.long_thread);
	free(linkmod_console);
}

bool linkmod_null_rx_is_available() {
	return true;
}

PigeonLinkmod *linkmod_null_rx_new() {
	LinkmodNull *linkmod_null = malloc(sizeof(LinkmodNull));
	memset(linkmod_null, 0, sizeof(*linkmod_null));
	linkmod_null->public.long_thread = long_thread_new((LongThreadOptions){
		.name="linkmod-null-rx",
		.data=linkmod_null
	});
	return (PigeonLinkmod *)linkmod_null;
}

/**
 * Free all the data we created in our constructor, including the LongThread
 * that we created.
 */
void linkmod_null_rx_free(PigeonLinkmod *linkmod) {
	LinkmodNull *linkmod_console = (LinkmodNull *)linkmod;
	long_thread_free(linkmod_console->public.long_thread);
	free(linkmod_console);
}
