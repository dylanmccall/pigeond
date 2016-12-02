#include "beagle_controls.h"

#include "long_thread.h"
#include "util.h"
#include "pigeon_ui.h"

#include <string.h>

struct _BeagleControls {
	LongThread *long_thread;
	PigeonFramePipe *pigeon_frame_pipe;
};

const struct timespec POLL_DELAY = {
	.tv_sec=0,
	.tv_nsec=100 * MILLISECONDS_IN_NANOSECONDS
};

LongThreadResult _beagle_controls_thread_loop(LongThread *long_thread, void *data);

BeagleControls *beagle_controls_new(PigeonFramePipe *pigeon_frame_pipe) {
	BeagleControls *beagle_controls = malloc(sizeof(BeagleControls));
	beagle_controls->pigeon_frame_pipe = pigeon_frame_pipe;
	beagle_controls->long_thread = long_thread_new((LongThreadOptions){
		.name="commandserver",
		.loop_fn=_beagle_controls_thread_loop,
		.data=beagle_controls
	});
	return beagle_controls;
}

void beagle_controls_free(BeagleControls *beagle_controls) {
	long_thread_free(beagle_controls->long_thread);
	free(beagle_controls);
}

bool beagle_controls_start(BeagleControls *beagle_controls) {
	return long_thread_start(beagle_controls->long_thread);
}

bool beagle_controls_wait(BeagleControls *beagle_controls) {
	return long_thread_wait(beagle_controls->long_thread);
}

int beagle_controls_join(BeagleControls *beagle_controls) {
	return long_thread_join(beagle_controls->long_thread);
}

bool beagle_controls_stop(BeagleControls *beagle_controls) {
	return long_thread_stop(beagle_controls->long_thread);
}

bool beagle_controls_is_running(BeagleControls *beagle_controls) {
	return long_thread_is_running(beagle_controls->long_thread);
}

LongThreadResult _beagle_controls_thread_loop(LongThread *long_thread, void *data) {
	BeagleControls *beagle_controls = (BeagleControls *)data;

	if (pigeon_ui_is_reset_pressed()) {
		pigeon_frame_pipe_reset(beagle_controls->pigeon_frame_pipe);
		pigeon_ui_action(UI_ACTION_TX_RESET);
	}

	nanosleep(&POLL_DELAY, NULL);

	return LONG_THREAD_CONTINUE;
}
