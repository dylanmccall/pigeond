#ifndef _UTIL_H
#define _UTIL_H

/**
 * util.h
 * General-purpose utility functions and macros for Pigeond
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNUSED(x) (void)(x)

#define MINUTES_IN_SECONDS 60
#define SECONDS_IN_MILLISECONDS 1000
#define MILLISECONDS_IN_NANOSECONDS 1000000
#define SECONDS_IN_NANOSECONDS 1000000000

int timespec_to_milliseconds(struct timespec *time);
int milliseconds_to_timespec(int milliseconds, struct timespec *out_timespec);
int timespec_delta(struct timespec *start, struct timespec *end, struct timespec *out_delta);
int timespec_delta_milliseconds(struct timespec *start, struct timespec *end);

char *path_join(const char *path1, size_t path1_length, const char *path2, size_t path2_length);

#endif
