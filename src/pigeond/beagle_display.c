#include "beagle_display.h"

#include "fsd_char.h"
#include "gpio.h"
#include "long_thread.h"
#include "util.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define SCREEN_SIZE 2
#define SCREEN_MAX_VALUE 99
#define SCREEN_BUFFER_SIZE 3

#define GPIO_DIGIT_0_PIN_NUMBER 61
#define GPIO_DIGIT_1_PIN_NUMBER 44

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x20
#define I2C_REG_DIRA 0x00
#define I2C_REG_DIRB 0x01
#define I2C_REG_OUT_TOP 0x15
#define I2C_REG_OUT_BOTTOM 0x14

#define FRAMES_PER_SECOND 100

struct _BeagleDisplay {
	LongThread *long_thread;
	const FSDChar *display_fsd_chars[SCREEN_BUFFER_SIZE];
	const FSDChar *flash_fsd_chars[SCREEN_BUFFER_SIZE];
	GPIOPin *gpio_digits[SCREEN_BUFFER_SIZE];
	int flash_time_frames;
	int i2c_fd;
	struct timespec start_time;
};

const struct timespec DIGIT_DELAY = {
	.tv_sec=0,
	.tv_nsec=5 * MILLISECONDS_IN_NANOSECONDS
};

bool _beagle_display_thread_start(LongThread *long_thread, void *data);
bool _beagle_display_thread_stop(LongThread *long_thread, void *data);
LongThreadResult _beagle_display_thread_loop(LongThread *long_thread, void *data);
bool _beagle_display_show_fsd_char(BeagleDisplay *beagle_display, const FSDChar *fsd_char);
bool _write_i2c_reg(int i2c_fd, unsigned char reg_addr, unsigned char value);

BeagleDisplay *beagle_display_new() {
	BeagleDisplay *beagle_display = malloc(sizeof(BeagleDisplay));
	beagle_display->long_thread = long_thread_new((LongThreadOptions){
		.name="commandserver",
		.start_fn=_beagle_display_thread_start,
		.stop_fn=_beagle_display_thread_stop,
		.loop_fn=_beagle_display_thread_loop,
		.data=beagle_display
	});
	return beagle_display;
}

void beagle_display_free(BeagleDisplay *beagle_display) {
	long_thread_free(beagle_display->long_thread);
	free(beagle_display);
}

bool beagle_display_start(BeagleDisplay *beagle_display) {
	return long_thread_start(beagle_display->long_thread);
}

bool beagle_display_wait(BeagleDisplay *beagle_display) {
	return long_thread_wait(beagle_display->long_thread);
}

int beagle_display_join(BeagleDisplay *beagle_display) {
	return long_thread_join(beagle_display->long_thread);
}

bool beagle_display_stop(BeagleDisplay *beagle_display) {
	return long_thread_stop(beagle_display->long_thread);
}

bool beagle_display_is_running(BeagleDisplay *beagle_display) {
	return long_thread_is_running(beagle_display->long_thread);
}

bool _beagle_display_thread_start(LongThread *long_thread, void *data) {
	BeagleDisplay *beagle_display = (BeagleDisplay *)data;

	bool error = false;

	if (!error) {
		beagle_display->gpio_digits[0] = gpio_open(GPIO_DIGIT_0_PIN_NUMBER, GPIO_OUT);
		beagle_display->gpio_digits[1] = gpio_open(GPIO_DIGIT_1_PIN_NUMBER, GPIO_OUT);
		if (beagle_display->gpio_digits[0] == NULL || beagle_display->gpio_digits[1] == NULL) {
			error = true;
		}
	}

	if (!error) {
		beagle_display->i2c_fd = open(I2CDRV_LINUX_BUS1, O_RDWR);
		if (beagle_display->i2c_fd < 0) {
			perror("Error opening I2C bus");
			fprintf(stderr, "Bus %s\n", I2CDRV_LINUX_BUS1);
			error = true;
		}
	}

	if (!error) {
		if (ioctl(beagle_display->i2c_fd, I2C_SLAVE, I2C_DEVICE_ADDRESS) != 0) {
			perror("Error setting I2C device to slave address");
			error = true;
		}
	}

	if (!error) {
		bool success = true;
		success &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_DIRA, 0x00);
		success &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_DIRB, 0x00);
		if (!success) {
			perror("Error setting I2C output pins");
			error = true;
		}
	}

	if (!error) {
		beagle_display_set_output(beagle_display, "  ");
		clock_gettime(CLOCK_MONOTONIC, &beagle_display->start_time);
	}

	if (error) {
		if (beagle_display->gpio_digits[0]) {
			gpio_close(beagle_display->gpio_digits[0]);
			beagle_display->gpio_digits[0] = NULL;
		}
		if (beagle_display->gpio_digits[1]) {
			gpio_close(beagle_display->gpio_digits[1]);
			beagle_display->gpio_digits[1] = NULL;
		}
		if (beagle_display->i2c_fd) {
			close(beagle_display->i2c_fd);
		}
	}

	return !error;
}

bool _beagle_display_thread_stop(LongThread *long_thread, void *data) {
	BeagleDisplay *beagle_display = (BeagleDisplay *)data;

	bool error = false;

	if (!error) {
		gpio_write(beagle_display->gpio_digits[0], 0);
		gpio_write(beagle_display->gpio_digits[1], 0);
		_beagle_display_show_fsd_char(beagle_display, NULL);
	}

	if (!error) {
		gpio_close(beagle_display->gpio_digits[0]);
		beagle_display->gpio_digits[0] = NULL;
		gpio_close(beagle_display->gpio_digits[1]);
		beagle_display->gpio_digits[1] = NULL;
	}

	if (!error) {
		close(beagle_display->i2c_fd);
	}

	return !error;
}

int beagle_display_set_output(BeagleDisplay *beagle_display, const char *output_str) {
	return str_to_fsd_chars(output_str, beagle_display->display_fsd_chars, SCREEN_BUFFER_SIZE);
}

int beagle_display_set_flash(BeagleDisplay *beagle_display, const char *flash_str, int time_seconds) {
	beagle_display->flash_time_frames = time_seconds * FRAMES_PER_SECOND;
	return str_to_fsd_chars(flash_str, beagle_display->flash_fsd_chars, SCREEN_BUFFER_SIZE);
}

int beagle_display_set_output_number(BeagleDisplay *beagle_display, unsigned int value) {
	if (value > SCREEN_MAX_VALUE) value = SCREEN_MAX_VALUE;
	char output_str[SCREEN_BUFFER_SIZE];
	if (snprintf(output_str, SCREEN_BUFFER_SIZE, "%*d", SCREEN_SIZE, value) >= 0) {
		return beagle_display_set_output(beagle_display, output_str);
	} else {
		return -1;
	}
}

LongThreadResult _beagle_display_thread_loop(LongThread *long_thread, void *data) {
	BeagleDisplay *beagle_display = (BeagleDisplay *)data;

	struct timespec now;
	const FSDChar **fsd_chars;

	if (beagle_display->flash_time_frames > 0) {
		fsd_chars = beagle_display->flash_fsd_chars;
		beagle_display->flash_time_frames -= 1;
	} else {
		fsd_chars = beagle_display->display_fsd_chars;
	}

	clock_gettime(CLOCK_MONOTONIC, &now);

	// Left digit
	gpio_write(beagle_display->gpio_digits[0], 0);
	gpio_write(beagle_display->gpio_digits[1], 0);
	_beagle_display_show_fsd_char(beagle_display, fsd_chars[0]);
	gpio_write(beagle_display->gpio_digits[0], 1);
	nanosleep(&DIGIT_DELAY, NULL);

	// Right digit
	gpio_write(beagle_display->gpio_digits[0], 0);
	gpio_write(beagle_display->gpio_digits[1], 0);
	_beagle_display_show_fsd_char(beagle_display, fsd_chars[1]);
	gpio_write(beagle_display->gpio_digits[1], 1);
	nanosleep(&DIGIT_DELAY, NULL);

	return LONG_THREAD_CONTINUE;
}

bool _beagle_display_show_fsd_char(BeagleDisplay *beagle_display, const FSDChar *fsd_char) {
	bool result = true;
	if (fsd_char != NULL) {
		result &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_OUT_TOP, fsd_char->pixel_values_top);
		result &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_OUT_BOTTOM, fsd_char->pixel_values_bottom);
	} else {
		result &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_OUT_TOP, 0);
		result &= _write_i2c_reg(beagle_display->i2c_fd, I2C_REG_OUT_BOTTOM, 0);
	}
	return result;
}

bool _write_i2c_reg(int i2c_fd, unsigned char reg_addr, unsigned char value) {
	const unsigned char buffer[2] = {reg_addr, value};
	int result = write(i2c_fd, buffer, 2);
	if (result != 2) {
		perror("Unable to write i2c register\n");
		return false;
	} else {
		return true;
	}
}
