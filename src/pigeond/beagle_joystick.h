#ifndef _BEAGLE_JOYSTICK_H
#define _BEAGLE_JOYSTICK_H

/**
 * beagle_joystick.h
 * Helper functions to access the joystick on our BeagleBone device.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct _BeagleJoystick BeagleJoystick;

typedef struct _BeagleJoystickVectors BeagleJoystickVectors;

struct _BeagleJoystickVectors {
    short x;
    short y;
    short z;
};

BeagleJoystick *beagle_joystick_open();
void beagle_joystick_close(BeagleJoystick *beagle_joystick);
void beagle_joystick_get_motion(BeagleJoystick *beagle_joystick, BeagleJoystickVectors *out_vectors);

#endif
