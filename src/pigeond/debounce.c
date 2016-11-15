#include "debounce.h"

#include "util.h"

bool debounce_action(Debounce *debounce, bool pressed) {
	bool result = false;
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	if (pressed && !debounce->pressed) {
		// Started press
		if (timespec_delta_milliseconds(&debounce->start_time, &now) > DEBOUNCE_RELEASE_MS) {
			// Start a new press if DEBOUNCE_RELEASE_MS has passed the last press
			debounce->done = false;
			debounce->start_time = now;
		}
	} else if (pressed && debounce->pressed) {
		// Continuing press
		int delay = !debounce->done ? DEBOUNCE_DELAY_MS : DEBOUNCE_DELAY_2_MS;
		if (timespec_delta_milliseconds(&debounce->start_time, &now) > delay) {
			// Release after delay
			debounce->done = true;
			debounce->start_time = now;
			result = true;
		}
	} else if (!pressed && debounce->pressed) {
		// Releasing
		if (!debounce->done) {
			debounce->done = true;
			debounce->start_time = now;
			result = true;
		}
	} else {
		// No action
	}

	debounce->pressed = pressed;
	debounce->released = !pressed;

	return result;
}
