#include "gpio.h"

#include <assert.h>
#include <stdio.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_PORT_DIRECTION_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_PORT_VALUE_PATH "/sys/class/gpio/gpio%d/value"

struct _GPIOPin {
	int pin_number;
	gpio_direction direction;
};

FILE* _gpio_value_file(GPIOPin *gpio_pin);

GPIOPin *gpio_open(int pin_number, gpio_direction direction) {
	bool error = false;
	GPIOPin *gpio_pin = NULL;
	FILE *export_file = NULL;
	FILE *direction_file = NULL;
	char direction_path[255];
	const char *direction_str;

	if (direction == GPIO_IN) {
		direction_str = "in";
	} else if (direction == GPIO_OUT) {
		direction_str = "out";
	} else {
		error = true;
	}

	if (!error) {
		export_file = fopen(GPIO_EXPORT_PATH, "w");
		if (export_file == NULL) {
			perror("Error opening gpio export file");
			error = true;
		}
	}

	if (!error) {
		if (fprintf(export_file, "%d", pin_number) < 0) {
			perror("Error exporting gpio pin");
			fprintf(stderr, "Pin number %d\n", pin_number);
			error = true;
		}
	}

	if (export_file != NULL) {
		fclose(export_file);
	}

	if (!error) {
		if (sprintf(direction_path, GPIO_PORT_DIRECTION_PATH, pin_number) < 0) {
			error = true;
		}
	}

	if (!error) {
		direction_file = fopen(direction_path, "w");
		if (direction_file == NULL) {
			perror("Error opening gpio direction file");
			fprintf(stderr, "Pin number %d\n", pin_number);
			error = true;
		}
	}

	if (!error) {
		if (fprintf(direction_file, "%s", direction_str) < 0) {
			perror("Error setting gpio direction");
			fprintf(stderr, "Pin number %d\n", pin_number);
			error = true;
		}
	}

	if (direction_file != NULL) {
		fclose(direction_file);
	}

	if (!error) {
		gpio_pin = malloc(sizeof(GPIOPin));
		gpio_pin->pin_number = pin_number;
		gpio_pin->direction = direction;
	}

	return gpio_pin;
}

bool gpio_close(GPIOPin *gpio_pin) {
	bool error = false;
	FILE *unexport_file;

	if (!error) {
		unexport_file = fopen(GPIO_UNEXPORT_PATH, "w");
		if (unexport_file == NULL) {
			perror("Error opening gpio unexport file");
			error = true;
		}
	}

	if (!error) {
		if (fprintf(unexport_file, "%d", gpio_pin->pin_number) < 0) {
			perror("Error unexporting gpio pin");
			fprintf(stderr, "Pin number %d\n", gpio_pin->pin_number);
			error = true;
		}
	}

	if (!error) {
		free(gpio_pin);
	}

	if (unexport_file != NULL) {
		fclose(unexport_file);
	}

	return !error;
}

bool gpio_read(GPIOPin *gpio_pin, int *result) {
	assert(gpio_pin->direction == GPIO_IN);
	bool error = false;
	FILE *gpio_file = _gpio_value_file(gpio_pin);

	if (!error) {
		if (fscanf(gpio_file, "%d", result) < 0) {
			perror("Error reading from gpio value file");
			fprintf(stderr, "Pin number %d\n", gpio_pin->pin_number);
			error = true;
		}
	}

	if (gpio_file != NULL) {
		fclose(gpio_file);
	}

	return !error;
}

bool gpio_write(GPIOPin *gpio_pin, int value) {
	assert(gpio_pin->direction == GPIO_OUT);
	bool error = false;
	FILE *gpio_file = _gpio_value_file(gpio_pin);

	value = value ? 1 : 0;

	if (!error) {
		if (fprintf(gpio_file, "%d\n", value) < 0) {
			perror("Error reading from gpio value file");
			fprintf(stderr, "Pin number %d\n", gpio_pin->pin_number);
			error = true;
		}
	}

	if (gpio_file != NULL) {
		fclose(gpio_file);
	}

	return !error;
}

FILE* _gpio_value_file(GPIOPin *gpio_pin) {
	bool error = false;
	FILE *value_file = NULL;
	char value_path[255];
	const char *file_mode;

	if (gpio_pin->direction == GPIO_IN) {
		file_mode = "r";
	} else if (gpio_pin->direction == GPIO_OUT) {
		file_mode = "w";
	} else {
		error = true;
	}

	if (!error) {
		if (sprintf(value_path, GPIO_PORT_VALUE_PATH, gpio_pin->pin_number) < 0) {
			error = true;
		}
	}

	if (!error) {
		value_file = fopen(value_path, file_mode);
		if (value_file == NULL) {
			perror("Error opening gpio value file");
			fprintf(stderr, "Pin number %d\n", gpio_pin->pin_number);
			error = true;
		}
	}

	return value_file;
}
