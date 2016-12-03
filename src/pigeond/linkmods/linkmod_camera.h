#ifndef _LINKMOD_CAMERA_H
#define _LINKMOD_CAMERA_H

/**
 * linkmod_camera.h
 * Linkmod that reads barcodes from the webcam
 * @author Sunny Singh <psa71@sfu.ca>
 */

#include "../pigeon_linkmod.h"

#include <stdbool.h>
#include <stdlib.h>

bool linkmod_camera_tx_is_available();

bool linkmod_camera_rx_is_available();
PigeonLinkmod *linkmod_camera_rx_new();
void linkmod_camera_rx_free(PigeonLinkmod *linkmod);

#endif
