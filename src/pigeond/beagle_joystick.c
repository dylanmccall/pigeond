#include "beagle_joystick.h"

#include "gpio.h"

#include <assert.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	const char* name;
	int pin_number;
	short x;
	short y;
	short z;
} JoystickDirection;

const JoystickDirection DIRECTIONS[] = {
	{"up", 26, 0, 1, 0},
	{"right", 47, 1, 0, 0},
	{"down", 46, 0, -1, 0},
	{"left", 65, -1, 0, 0},
	{"center", 27, 0, 0, 1}
};

const size_t DIRECTIONS_COUNT = sizeof(DIRECTIONS) / sizeof(JoystickDirection);

struct _BeagleJoystick {
	GPIOPin **gpio;
};

bool _write_i2c_reg(int i2c_fd, unsigned char reg_addr, unsigned char value);
size_t _read_i2c_regs(int i2c_fd, unsigned char start_addr, unsigned char end_addr, unsigned char **out_buffer);
int16_t _int16_from_char_buffer(unsigned char *buffer, int lsb, int msb);

BeagleJoystick *beagle_joystick_open() {
	BeagleJoystick *beagle_joystick = malloc(sizeof(BeagleJoystick));
	memset(beagle_joystick, 0, sizeof(*beagle_joystick));

	bool error = false;

	if (!error) {
		beagle_joystick->gpio = calloc(DIRECTIONS_COUNT, sizeof(*beagle_joystick->gpio));
		if (beagle_joystick->gpio == NULL) {
			perror("Error allocating space for array");
			error = true;
		}
	}

	if (!error) {
		for (int i = 0; i < DIRECTIONS_COUNT; i++) {
			const JoystickDirection *direction = &DIRECTIONS[i];
			GPIOPin *direction_pin = gpio_open(direction->pin_number, GPIO_IN);
			beagle_joystick->gpio[i] = direction_pin;
			if (direction_pin == NULL) {
				error = true;
			}
		}
	}

	if (error) {
		free(beagle_joystick);
		beagle_joystick = NULL;
	}

	return beagle_joystick;
}

void beagle_joystick_close(BeagleJoystick *beagle_joystick) {
	for (int i = 0; i < DIRECTIONS_COUNT; i++) {
		GPIOPin *direction_pin = beagle_joystick->gpio[i];
		gpio_close(direction_pin);
		beagle_joystick->gpio[i] = NULL;
	}
	free(beagle_joystick->gpio);
	free(beagle_joystick);
}

void beagle_joystick_get_motion(BeagleJoystick *beagle_joystick, BeagleJoystickVectors *out_vectors) {
	out_vectors->x = 0;
	out_vectors->y = 0;
	out_vectors->z = 0;

	for (int i = 0; i < DIRECTIONS_COUNT; i++) {
		const JoystickDirection *direction = &DIRECTIONS[i];
		GPIOPin *direction_pin = beagle_joystick->gpio[i];
		int value;
		gpio_read(direction_pin, &value);
		if (value == 0) {
			out_vectors->x += direction->x;
			out_vectors->y += direction->y;
			out_vectors->z += direction->z;
		}
	}
}
