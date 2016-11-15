#ifndef _DEBOUNCE_H
#define _DEBOUNCE_H

/**
 * debounce.h
 * Debounce arbitrary input events.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBOUNCE_DELAY_MS 800
#define DEBOUNCE_DELAY_2_MS 250
#define DEBOUNCE_RELEASE_MS 250

typedef struct _Debounce Debounce;

typedef enum {ON_PRESS, ON_PRESS_REPEAT, ON_RELEASE} debounce_mode;

struct _Debounce {
    debounce_mode mode;
    bool pressed;
    bool released;
    struct timespec start_time;
    bool done;
};

bool debounce_action(Debounce *debounce, bool pressed);

#endif
