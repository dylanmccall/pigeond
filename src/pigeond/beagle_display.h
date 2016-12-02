#ifndef _BEAGLE_DISPLAY_H
#define _BEAGLE_DISPLAY_H

/**
 * beagle_display.h
 * Updates a 14-segment display attached to our BeagleBone
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _BeagleDisplay BeagleDisplay;

BeagleDisplay *beagle_display_new();
void beagle_display_free(BeagleDisplay *beagle_display);
bool beagle_display_start(BeagleDisplay *beagle_display);
bool beagle_display_wait(BeagleDisplay *beagle_display);
int beagle_display_join(BeagleDisplay *beagle_display);
bool beagle_display_stop(BeagleDisplay *beagle_display);
bool beagle_display_is_running(BeagleDisplay *beagle_display);
int beagle_display_set_flash(BeagleDisplay *beagle_display, const char *flash_str, int time_seconds);
int beagle_display_set_output(BeagleDisplay *beagle_display, const char *output_str);
int beagle_display_set_output_number(BeagleDisplay *beagle_display, unsigned int value);

#endif
