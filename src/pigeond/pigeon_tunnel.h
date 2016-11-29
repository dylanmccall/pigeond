#ifndef _PIGEON_TUNNEL_H
#define _PIGEON_TUNNEL_H

/**
 * pigeon_tunnel.h
 * Manages a network tunnel device to communicate with a PigeonFramePipe.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "pigeon_frame.h"
#include "pigeon_frame_pipe.h"

typedef struct _PigeonTunnel PigeonTunnel;

PigeonTunnel *pigeon_tunnel_new(const char *dev_name_template, PigeonFramePipeHandle frame_pipe_ref_tx);
void pigeon_tunnel_free(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_init(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_start(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_wait(PigeonTunnel *pigeon_tunnel);
int pigeon_tunnel_join(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_stop(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_is_running(PigeonTunnel *pigeon_tunnel);

const char *pigeon_tunnel_get_dev_name(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_set_mtu(PigeonTunnel *pigeon_tunnel, int mtu);
int pigeon_tunnel_get_mtu(PigeonTunnel *pigeon_tunnel);

bool pigeon_tunnel_frames_wait(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_frames_has_next(PigeonTunnel *pigeon_tunnel);
PigeonFrame *pigeon_tunnel_frames_pop(PigeonTunnel *pigeon_tunnel);
bool pigeon_tunnel_frames_push(PigeonTunnel *pigeon_tunnel, PigeonFrame *pigeon_frame);

#endif
