#include "util.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

int timespec_to_milliseconds(struct timespec *time) {
	int result = 0;
	result += time->tv_sec * SECONDS_IN_MILLISECONDS;
	result += time->tv_nsec / MILLISECONDS_IN_NANOSECONDS;
	return result;
}

int milliseconds_to_timespec(int milliseconds, struct timespec *out_timespec) {
	time_t seconds = (int) (milliseconds / SECONDS_IN_MILLISECONDS);
	long remaining_ms = milliseconds - ((long) seconds * SECONDS_IN_MILLISECONDS);
	out_timespec->tv_sec = seconds;
	out_timespec->tv_nsec = remaining_ms * MILLISECONDS_IN_NANOSECONDS;
	return 0;
}

int timespec_delta(struct timespec *start, struct timespec *end, struct timespec *out_delta) {
	if ((end->tv_nsec - start->tv_nsec) < 0) {
		out_delta->tv_sec = end->tv_sec - start->tv_sec - 1;
		out_delta->tv_nsec = SECONDS_IN_NANOSECONDS + end->tv_nsec - start->tv_nsec;
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

char *path_join(const char *path1, size_t path1_length, const char *path2, size_t path2_length) {
	char *path_str;

	path_str = calloc(path1_length + 1 + path2_length, sizeof(*path_str));
	strncat(path_str, path1, path1_length);
	strncat(path_str, "/", 1);
	strncat(path_str, path2, path2_length);

	return path_str;
}
