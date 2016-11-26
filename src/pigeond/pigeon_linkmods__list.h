#ifndef _PIGEON_LINKMODS__LIST_H
#define _PIGEON_LINKMODS__LIST_H

/**
 * pigeon_linkmods__list.h
 * List of linkmod options. Edit this file to add new linkmod options.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "pigeon_linkmod.h"

#include "linkmods/linkmod_console.h"

// Link tx modes: File, Print, Console.
// File:  Watch a particular (configured) directory. Write to there if it
//        exists (and flush). Wait if it does not. Beep after writing.
// Print: Send to printer immediately if possible. Wait if not.
//        (Possibly listen for a button press and show unsent packages on
//        screen). Beep after printing.

// Link rx modes: USB, Scan, Console.
// File:   Watch a particular (configured) directory. Read from there if it
//         exists. Otherwise do nothing. Beep after reading.
// Scan:   Listen for (any!) joystick event. Scan from camera if possible.
//         Otherwise do nothing. (Try to poll the camera continuously).
//         Beep after scanning.

const PigeonLinkmodInfo LINKMODS_TX[] = {
	{
		.type = LINKMOD_TYPE_TX,
		.name = "print-tx",
		.is_available_fn = NULL,
		.new_fn = NULL,
		.free_fn = NULL
	},
	{
		.type = LINKMOD_TYPE_TX,
		.name = "file-tx",
		.is_available_fn = NULL,
		.new_fn = NULL,
		.free_fn = NULL
	},
	{
		.type = LINKMOD_TYPE_TX,
		.name = "console-tx",
		.is_available_fn = linkmod_console_tx_is_available,
		.new_fn = linkmod_console_tx_new,
		.free_fn = linkmod_console_tx_free
	}
};

const size_t LINKMODS_TX_COUNT = sizeof(LINKMODS_TX) / sizeof(*LINKMODS_TX);

const PigeonLinkmodInfo LINKMODS_RX[] = {
	{
		.type = LINKMOD_TYPE_RX,
		.name = "scan-rx",
		.is_available_fn = NULL,
		.new_fn = NULL,
		.free_fn = NULL
	},
	{
		.type = LINKMOD_TYPE_RX,
		.name = "file-rx",
		.is_available_fn = NULL,
		.new_fn = NULL,
		.free_fn = NULL
	},
	{
		.type = LINKMOD_TYPE_RX,
		.name = "console-rx",
		.is_available_fn = linkmod_console_rx_is_available,
		.new_fn = linkmod_console_rx_new,
		.free_fn = linkmod_console_rx_free
	}
};

const size_t LINKMODS_RX_COUNT = sizeof(LINKMODS_RX) / sizeof(*LINKMODS_RX);

#endif
