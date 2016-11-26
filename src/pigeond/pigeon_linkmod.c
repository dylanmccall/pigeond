#include "pigeon_linkmod.h"

#include "pigeon_linkmods__list.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

const PigeonLinkmodInfo *_pigeon_linkmod_find_available(const PigeonLinkmodInfo *linkmods, size_t linkmods_count);
bool _pigeon_linkmod_is_available(const PigeonLinkmodInfo *linkmod_info);
LongThread *_pigeon_linkmod_new_thread(const PigeonLinkmodInfo *linkmod_info, PigeonLink *pigeon_link);

PigeonLinkmod *pigeon_linkmod_new_tx(PigeonLink *pigeon_link) {
	const PigeonLinkmodInfo *linkmod_info = _pigeon_linkmod_find_available(LINKMODS_TX, LINKMODS_TX_COUNT);
	return pigeon_linkmod_new_from_info(linkmod_info, pigeon_link);
}

PigeonLinkmod *pigeon_linkmod_new_rx(PigeonLink *pigeon_link) {
	const PigeonLinkmodInfo *linkmod_info = _pigeon_linkmod_find_available(LINKMODS_RX, LINKMODS_RX_COUNT);
	return pigeon_linkmod_new_from_info(linkmod_info, pigeon_link);
}

PigeonLinkmod *pigeon_linkmod_new_from_info(const PigeonLinkmodInfo *linkmod_info, PigeonLink *pigeon_link) {
	if (linkmod_info != NULL) {
		PigeonLinkmod *linkmod = malloc(sizeof(PigeonLinkmod));
		memset(linkmod, 0, sizeof(*linkmod));
		linkmod->linkmod_info = linkmod_info;
		linkmod->long_thread = _pigeon_linkmod_new_thread(linkmod_info, pigeon_link);
		return linkmod;
	} else {
		return NULL;
	}
}

void pigeon_linkmod_free(PigeonLinkmod *pigeon_linkmod) {
	long_thread_free(pigeon_linkmod->long_thread);
	free(pigeon_linkmod);
}

bool pigeon_linkmod_start(PigeonLinkmod *pigeon_linkmod) {
	return long_thread_start(pigeon_linkmod->long_thread);
}

bool pigeon_linkmod_wait(PigeonLinkmod *pigeon_linkmod) {
	return long_thread_wait(pigeon_linkmod->long_thread);
}

int pigeon_linkmod_join(PigeonLinkmod *pigeon_linkmod) {
	return long_thread_join(pigeon_linkmod->long_thread);
}

bool pigeon_linkmod_stop(PigeonLinkmod *pigeon_linkmod) {
	return long_thread_stop(pigeon_linkmod->long_thread);
}

bool pigeon_linkmod_is_running(PigeonLinkmod *pigeon_linkmod) {
	return long_thread_is_running(pigeon_linkmod->long_thread);
}

const char *pigeon_linkmod_get_name(PigeonLinkmod *pigeon_linkmod) {
	const PigeonLinkmodInfo *linkmod_info = pigeon_linkmod->linkmod_info;
	if (linkmod_info) {
		return linkmod_info->name;
	} else {
		return NULL;
	}
}

const PigeonLinkmodInfo *_pigeon_linkmod_find_available(const PigeonLinkmodInfo *linkmods, size_t options_count) {
	for (size_t i = 0; i < options_count; i++) {
		const PigeonLinkmodInfo *linkmod_info = &linkmods[i];
		if (_pigeon_linkmod_is_available(linkmod_info)) {
			return linkmod_info;
		}
	}
	return NULL;
}

bool _pigeon_linkmod_is_available(const PigeonLinkmodInfo *linkmod_info) {
	if (linkmod_info->is_available_fn != NULL) {
		return linkmod_info->is_available_fn();
	} else {
		return false;
	}
}

LongThread *_pigeon_linkmod_new_thread(const PigeonLinkmodInfo *linkmod_info, PigeonLink *pigeon_link) {
	assert(linkmod_info->new_thread_fn);
	return linkmod_info->new_thread_fn(pigeon_link);
}
