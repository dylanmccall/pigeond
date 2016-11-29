#ifndef _LINKMOD_PRINTER_H
#define _LINKMOD_PRINTER_H

/**
 * linkmod_printer.h
 * Linkmod that encodes packets as QR codes and prints them on a thermal printer
 * @author Adam Shambrook <ashambro@sfu.ca>
 */

#include "../pigeon_linkmod.h"

#include <stdbool.h>
#include <stdlib.h>

bool linkmod_printer_tx_is_available();
PigeonLinkmod *linkmod_printer_tx_new();
void linkmod_printer_tx_free(PigeonLinkmod *linkmod);

bool linkmod_printer_rx_is_available();

#endif
