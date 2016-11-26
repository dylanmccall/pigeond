#ifndef _PIGEON_LINKMOD_CONSOLE_H
#define _PIGEON_LINKMOD_CONSOLE_H

/**
 * pigeon_linkmod_console.h
 * Linkmod that echoes packets to the console.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "../pigeon_linkmod.h"

#include <stdbool.h>
#include <stdlib.h>

bool linkmod_console_tx_is_available();
PigeonLinkmod *linkmod_console_tx_new();
void linkmod_console_tx_free(PigeonLinkmod *linkmod);

bool linkmod_console_rx_is_available();
PigeonLinkmod *linkmod_console_rx_new();
void linkmod_console_rx_free(PigeonLinkmod *linkmod);

#endif
