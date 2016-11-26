#ifndef _PIGEON_LINK_H
#define _PIGEON_LINK_H

/**
 * pigeon_link.h
 * Asynchronously send or receive ethernet frames over the physical layer.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "pigeon_frame.h"

#define PIGEON_LINK_MTU 300
#define PIGEON_RX_BUFFER_SIZE 16
#define PIGEON_TX_BUFFER_SIZE 16

typedef struct _PigeonLink PigeonLink;

PigeonLink *pigeon_link_new();
void pigeon_link_free(PigeonLink *pigeon_link);
bool pigeon_link_init(PigeonLink *pigeon_link);
bool pigeon_link_start(PigeonLink *pigeon_link);
bool pigeon_link_wait(PigeonLink *pigeon_link);
int pigeon_link_join(PigeonLink *pigeon_link);
bool pigeon_link_stop(PigeonLink *pigeon_link);
bool pigeon_link_is_running(PigeonLink *pigeon_link);

void pigeon_link_print_debug_info(PigeonLink *pigeon_link);

bool pigeon_link_tx_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame);
size_t pigeon_link_tx_count(PigeonLink *pigeon_link);
// Get the next frame to send. The caller is now the owner of the frame and
// must call pigeon_frame_free when finished.
PigeonFrame *pigeon_link_tx_pop(PigeonLink *pigeon_link);

bool pigeon_link_rx_push(PigeonLink *pigeon_link, PigeonFrame *pigeon_frame);
size_t pigeon_link_rx_count(PigeonLink *pigeon_link);
// Get the next frame received. The caller is now the owner of the frame and
// must call pigeon_frame_free when finished.
PigeonFrame *pigeon_link_rx_pop(PigeonLink *pigeon_link);

#endif
