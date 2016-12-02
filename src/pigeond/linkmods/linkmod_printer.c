#include "linkmod_printer.h"

#include <stdio.h>
#include <string.h>
#include "../printer.h"
#include <unistd.h>
#include <fcntl.h>
#include "../audioMixer.h"

//#define PRINTER_FILE "./test.txt"
#define PRINTER_FILE "/dev/ttyO5"

/**
 * A linkmod porovides an abstract interface for our modem to send and receive
 * data. This is necessary because the same data can be carried over a variety
 * of mediums. Linkmods are specified, in order, in pigeon_linkmods__list.h.
 * Every linkmod can be either a transmit (TX) or receive (RX) module. It has
 * a name, a function that says whether it should be used (is_available_fn), a
 * constructor (new_fn), and a destructor (free_fn).
 */

/**
 * Data used in this module, as well as public data that is shared with the
 * rest of the application. Every linkmod should create a struct with a
 * PigeonLinkmod as its first parameter.
 */
typedef struct {
	PigeonLinkmod public;
	int fileDescriptor;
	wavedata_t *testSound;
} LinkmodPrinter;

bool _linkmod_printer_tx_thread_start(LongThread *long_thread, void *data);
bool _linkmod_printer_tx_thread_stop(LongThread *long_thread, void *data);
LongThreadResult _linkmod_printer_tx_thread_loop(LongThread *long_thread, void *data);

/**
 * This should check for available hardware and only return true if this
 * linkmod will work as expected. PigeonLink will select the first linkmod
 * that says it is available.
 */
bool linkmod_printer_tx_is_available() {
	return true;
	// if( access(PRINTER_FILE, F_OK) != -1) {
	// 	return true;
	// }
	// else {
	// 	return false;
	// }
}

/**
 * Create a LinkmodPrinter struct for our own use, and return it as a pointer
 * to PigeonLinkmod. It is important that we fill in the public long_thread
 * field - the caller expects us to create one. (This isn't ideal, but it was
 * a lot less work and I should probably go to sleep at some point).
 */
PigeonLinkmod *linkmod_printer_tx_new() {
	LinkmodPrinter *linkmod_printer = malloc(sizeof(LinkmodPrinter));
	memset(linkmod_printer, 0, sizeof(*linkmod_printer));
	linkmod_printer->testSound = AudioMixer_waveData_new();
	linkmod_printer->public.long_thread = long_thread_new((LongThreadOptions){
		.name="linkmod-printer-tx",
		.start_fn=_linkmod_printer_tx_thread_start,
		.stop_fn=_linkmod_printer_tx_thread_stop,
		.loop_fn=_linkmod_printer_tx_thread_loop,
		.data=linkmod_printer
	}); 
	return (PigeonLinkmod *)linkmod_printer;
}

/**
 * Free all the data we created in our constructor, including the LongThread
 * that we created.
 */
void linkmod_printer_tx_free(PigeonLinkmod *linkmod) {
	LinkmodPrinter *linkmod_printer = (LinkmodPrinter *)linkmod;
	long_thread_free(linkmod_printer->public.long_thread);
	AudioMixer_waveData_free(linkmod_printer->testSound);
	free(linkmod_printer);
}

bool _linkmod_printer_tx_thread_start(LongThread *long_thread, void *data) {
	LinkmodPrinter *linkmod_printer = (LinkmodPrinter *)data;

	bool error = false;

	if (!error) {
		// Any expensive initialization. (Open files, etc.).
		linkmod_printer->fileDescriptor = open(PRINTER_FILE, O_WRONLY | O_NOCTTY | O_NDELAY);
		AudioMixer_readWaveFileIntoMemory("data/SoundEffects/testSound.wav", linkmod_printer->testSound);
	}

	return !error;
}

bool _linkmod_printer_tx_thread_stop(LongThread *long_thread, void *data) {
	LinkmodPrinter *linkmod_printer = (LinkmodPrinter *)data;

	bool error = false;

	if (!error) {
		// Close files opened in _linkmod_console_tx_thread_start.
		close(linkmod_printer->fileDescriptor);
		AudioMixer_freeWaveFileData(linkmod_printer->testSound);
	}

	return !error;
}

/**
 * Main loop callback for our LongThread. Avoid doing anything too expensive
 * here. If we need to do some initialization, add a start_fn and stop_fn
 * callback when we create our LongThread in the constructor.
 */
LongThreadResult _linkmod_printer_tx_thread_loop(LongThread *long_thread, void *data) {
	LinkmodPrinter *linkmod_printer = (LinkmodPrinter *)data;
	PigeonLink *pigeon_link = linkmod_printer->public.pigeon_link;

	PigeonFrame *pigeon_frame = pigeon_link_frames_pop(pigeon_link);

	if (pigeon_frame) {
		fprintf(stderr, "Printing frame\n");
		const unsigned char *toPrint;
		size_t toPrintLength = pigeon_frame_get_buffer(pigeon_frame, &toPrint);
		AudioMixer_queueSound(linkmod_printer->testSound);
		printer_printQRCode(linkmod_printer->fileDescriptor, toPrint, (int)toPrintLength);
		//pigeon_frame_print_header(pigeon_frame);
		//pigeon_frame_print_data(pigeon_frame);
		pigeon_frame_free(pigeon_frame);
	}

	return LONG_THREAD_CONTINUE;
}
