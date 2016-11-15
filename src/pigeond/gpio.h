#ifndef _GPIO_H
#define _GPIO_H

/**
 * gpio.h
 * A simple interface to access GPIO pins
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _GPIOPin GPIOPin;

typedef enum {GPIO_IN, GPIO_OUT} gpio_direction;

GPIOPin *gpio_open(int pin_number, gpio_direction direction);
bool gpio_close(GPIOPin *gpio_pin);
bool gpio_read(GPIOPin *gpio_pin, int *result);
bool gpio_write(GPIOPin *gpio_pin, int value);

#endif
