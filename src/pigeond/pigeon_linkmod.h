#ifndef _PIGEON_LINKMOD_H
#define _PIGEON_LINKMOD_H

/**
 * pigeon_linkmod.h
 * Custom send and receive modules for PigeonLink.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

#include "long_thread.h"
#include "pigeon_link.h"

typedef struct _PigeonLinkmod PigeonLinkmod;
typedef struct _PigeonLinkmodInfo PigeonLinkmodInfo;

typedef enum {
    LINKMOD_TYPE_NULL = 0,
    LINKMOD_TYPE_TX,
    LINKMOD_TYPE_RX
} PigeonLinkmodType;

typedef bool (*pigeon_linkmod_is_available_t)(void);
typedef PigeonLinkmod *(*pigeon_linkmod_new_t)();
typedef void (*pigeon_linkmod_free_t)(PigeonLinkmod *linkmod);

// TODO: It would be good to specify an MTU here so we can switch MTUs
//       based on the mode. That would also require some work in pigeon_tunnel
//       and elsewhere to support different MTUs for send and receive.

struct _PigeonLinkmod {
	const PigeonLinkmodInfo *linkmod_info;
	PigeonLink *pigeon_link;
	LongThread *long_thread;
};

struct _PigeonLinkmodInfo {
	PigeonLinkmodType type;
	const char *name;
	pigeon_linkmod_is_available_t is_available_fn;
	pigeon_linkmod_new_t new_fn;
	pigeon_linkmod_free_t free_fn;
};

PigeonLinkmod *pigeon_linkmod_new_tx(PigeonLink *pigeon_link);
PigeonLinkmod *pigeon_linkmod_new_rx(PigeonLink *pigeon_link);

void pigeon_linkmod_free(PigeonLinkmod *linkmod);
bool pigeon_linkmod_start(PigeonLinkmod *linkmod);
bool pigeon_linkmod_wait(PigeonLinkmod *linkmod);
int pigeon_linkmod_join(PigeonLinkmod *linkmod);
bool pigeon_linkmod_stop(PigeonLinkmod *linkmod);
bool pigeon_linkmod_is_running(PigeonLinkmod *linkmod);

const char *pigeon_linkmod_get_name(PigeonLinkmod *linkmod);

#endif
