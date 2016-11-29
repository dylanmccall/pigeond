#ifndef _LINKMOD_FILES_H
#define _LINKMOD_FILES_H

/**
 * linkmod_files.h
 * Linkmod that transmits and receives packets in a directory.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "../pigeon_linkmod.h"

#include <stdbool.h>
#include <stdlib.h>

bool linkmod_files_tx_is_available();
PigeonLinkmod *linkmod_files_tx_new();
void linkmod_files_tx_free(PigeonLinkmod *linkmod);

bool linkmod_files_rx_is_available();
PigeonLinkmod *linkmod_files_rx_new();
void linkmod_files_rx_free(PigeonLinkmod *linkmod);

#endif
