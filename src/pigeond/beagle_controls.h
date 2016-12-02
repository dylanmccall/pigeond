#ifndef _BEAGLE_CONTROLS_H
#define _BEAGLE_CONTROLS_H

/**
 * beagle_controls.h
 * Watches hardare controls to perform UI actions.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include "pigeon_frame_pipe.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct _BeagleControls BeagleControls;

BeagleControls *beagle_controls_new(PigeonFramePipe *pigeon_frame_pipe);
void beagle_controls_free(BeagleControls *beagle_controls);
bool beagle_controls_start(BeagleControls *beagle_controls);
bool beagle_controls_wait(BeagleControls *beagle_controls);
int beagle_controls_join(BeagleControls *beagle_controls);
bool beagle_controls_stop(BeagleControls *beagle_controls);
bool beagle_controls_is_running(BeagleControls *beagle_controls);

#endif
