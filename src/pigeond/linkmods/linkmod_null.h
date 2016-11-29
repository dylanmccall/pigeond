#ifndef _LINKMOD_NULL_H
#define _LINKMOD_NULL_H

/**
 * linkmod_null.h
 * Linkmod that does nothing.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "../pigeon_linkmod.h"

#include <stdbool.h>
#include <stdlib.h>

bool linkmod_null_tx_is_available();
PigeonLinkmod *linkmod_null_tx_new();
void linkmod_null_tx_free(PigeonLinkmod *linkmod);

bool linkmod_null_rx_is_available();
PigeonLinkmod *linkmod_null_rx_new();
void linkmod_null_rx_free(PigeonLinkmod *linkmod);

#endif
