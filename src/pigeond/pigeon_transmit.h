#ifndef _PIGEON_TRANSMIT_H
#define _PIGEON_TRANSMIT_H

/**
 * pigeon_transmit.h
 * Asynchronously send or receive ethernet frames over the physical layer.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "pigeon_frame.h"

#define PIGEON_TRANSMIT_MTU 300

typedef struct _PigeonTransmit PigeonTransmit;

PigeonTransmit *pigeon_transmit_new();
void pigeon_transmit_free(PigeonTransmit *pigeon_transmit);
bool pigeon_transmit_start(PigeonTransmit *pigeon_transmit);
bool pigeon_transmit_wait(PigeonTransmit *pigeon_transmit);
int pigeon_transmit_join(PigeonTransmit *pigeon_transmit);
bool pigeon_transmit_stop(PigeonTransmit *pigeon_transmit);
bool pigeon_transmit_is_running(PigeonTransmit *pigeon_transmit);

int pigeon_transmit_send(PigeonTransmit *pigeon_transmit, PigeonFrame *pigeon_frame);
PigeonFrame *pigeon_transmit_receive(PigeonTransmit *pigeon_transmit);
size_t pigeon_transmit_get_pending(PigeonTransmit *pigeon_transmit);

#endif
