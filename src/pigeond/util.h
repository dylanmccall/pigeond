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
#define NANOSECONDS_IN_MILLISECONDS 1000000
#define NANOSECONDS_IN_SECONDS 1000000000

int timespec_to_milliseconds(struct timespec *time);
int milliseconds_to_timespec(int milliseconds, struct timespec *out_timespec);
int timespec_delta(struct timespec *start, struct timespec *end, struct timespec *out_delta);
int timespec_delta_milliseconds(struct timespec *start, struct timespec *end);

#endif
