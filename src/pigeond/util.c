#include "util.h"

#include <limits.h>
#include <stdio.h>

int timespec_to_milliseconds(struct timespec *time) {
	int result = 0;
	result += time->tv_sec * SECONDS_IN_MILLISECONDS;
	result += time->tv_nsec / NANOSECONDS_IN_MILLISECONDS;
	return result;
}

int milliseconds_to_timespec(int milliseconds, struct timespec *out_timespec) {
	time_t seconds = (int) (milliseconds / SECONDS_IN_MILLISECONDS);
	long remaining_ms = milliseconds - ((long) seconds * SECONDS_IN_MILLISECONDS);
	out_timespec->tv_sec = seconds;
	out_timespec->tv_nsec = remaining_ms * NANOSECONDS_IN_MILLISECONDS;
	return 0;
}

int timespec_delta(struct timespec *start, struct timespec *end, struct timespec *out_delta) {
	if ((end->tv_nsec - start->tv_nsec) < 0) {
		out_delta->tv_sec = end->tv_sec - start->tv_sec - 1;
		out_delta->tv_nsec = NANOSECONDS_IN_SECONDS + end->tv_nsec - start->tv_nsec;
	} else {
		out_delta->tv_sec = end->tv_sec-start->tv_sec;
		out_delta->tv_nsec = end->tv_nsec-start->tv_nsec;
	}
	return 0;
}

int timespec_delta_milliseconds(struct timespec *start, struct timespec *end) {
	struct timespec delta;
	timespec_delta(start, end, &delta);
	return timespec_to_milliseconds(&delta);
}
