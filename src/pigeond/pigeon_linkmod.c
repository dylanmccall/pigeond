#include "pigeon_linkmod.h"

#include "pigeon_linkmods__list.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

const PigeonLinkmodInfo *_pigeon_linkmod_info_find_available(const PigeonLinkmodInfo *linkmods, size_t linkmods_count);
bool _pigeon_linkmod_info_is_available(const PigeonLinkmodInfo *linkmod_info);

PigeonLinkmod *_pigeon_linkmod_new_from_info(const PigeonLinkmodInfo *linkmod_info, PigeonLink *pigeon_link);
void _pigeon_linkmod_free_from_info(const PigeonLinkmodInfo *linkmod_info, PigeonLinkmod *linkmod);

PigeonLinkmod *pigeon_linkmod_new_tx(PigeonLink *pigeon_link) {
	const PigeonLinkmodInfo *linkmod_info = _pigeon_linkmod_info_find_available(LINKMODS_TX, LINKMODS_TX_COUNT);
	if (linkmod_info) {
		return _pigeon_linkmod_new_from_info(linkmod_info, pigeon_link);
	} else {
		return NULL;
	}
}

PigeonLinkmod *pigeon_linkmod_new_rx(PigeonLink *pigeon_link) {
	const PigeonLinkmodInfo *linkmod_info = _pigeon_linkmod_info_find_available(LINKMODS_RX, LINKMODS_RX_COUNT);
	if (linkmod_info) {
		return _pigeon_linkmod_new_from_info(linkmod_info, pigeon_link);
	} else {
		return NULL;
	}
}

void pigeon_linkmod_free(PigeonLinkmod *linkmod) {
	const PigeonLinkmodInfo *linkmod_info = linkmod->linkmod_info;
	if (linkmod_info) {
		_pigeon_linkmod_free_from_info(linkmod_info, linkmod);
	}
}

bool pigeon_linkmod_start(PigeonLinkmod *linkmod) {
	return long_thread_start(linkmod->long_thread);
}

bool pigeon_linkmod_wait(PigeonLinkmod *linkmod) {
	return long_thread_wait(linkmod->long_thread);
}

int pigeon_linkmod_join(PigeonLinkmod *linkmod) {
	return long_thread_join(linkmod->long_thread);
}

bool pigeon_linkmod_stop(PigeonLinkmod *linkmod) {
	return long_thread_stop(linkmod->long_thread);
}

bool pigeon_linkmod_is_running(PigeonLinkmod *linkmod) {
	return long_thread_is_running(linkmod->long_thread);
}

const char *pigeon_linkmod_get_name(PigeonLinkmod *linkmod) {
	const PigeonLinkmodInfo *linkmod_info = linkmod->linkmod_info;
	if (linkmod_info) {
		return linkmod_info->name;
	} else {
		return NULL;
	}
}

const PigeonLinkmodInfo *_pigeon_linkmod_info_find_available(const PigeonLinkmodInfo *linkmods, size_t options_count) {
	for (size_t i = 0; i < options_count; i++) {
		const PigeonLinkmodInfo *linkmod_info = &linkmods[i];
		if (_pigeon_linkmod_info_is_available(linkmod_info)) {
			return linkmod_info;
		}
	}
	return NULL;
}

bool _pigeon_linkmod_info_is_available(const PigeonLinkmodInfo *linkmod_info) {
	if (linkmod_info->is_available_fn != NULL) {
		return linkmod_info->is_available_fn();
	} else {
		return false;
	}
}

PigeonLinkmod *_pigeon_linkmod_new_from_info(const PigeonLinkmodInfo *linkmod_info, PigeonLink *pigeon_link) {
	assert(linkmod_info->new_fn);
	PigeonLinkmod *linkmod = linkmod_info->new_fn();
	linkmod->linkmod_info = linkmod_info;
	linkmod->pigeon_link = pigeon_link;
	assert(linkmod->long_thread);
	return linkmod;
}

void _pigeon_linkmod_free_from_info(const PigeonLinkmodInfo *linkmod_info, PigeonLinkmod *linkmod) {
	assert(linkmod_info->free_fn);
	linkmod_info->free_fn(linkmod);
}
