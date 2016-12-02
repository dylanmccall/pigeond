#ifndef _PIGEON_UI_H
#define _PIGEON_UI_H

/**
 * pigeon_ui.h
 * Common UI operations triggered throughout the application.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _PigeonUI PigeonUI;

typedef enum {
	UI_ACTION_RX_BUSY,
	UI_ACTION_RX_WAITING,
	UI_ACTION_RX_SUCCESS,
	UI_ACTION_RX_ERROR,
	UI_ACTION_TX_BUSY,
	UI_ACTION_TX_BUSY_SLOW,
	UI_ACTION_TX_WAITING,
	UI_ACTION_TX_SUCCESS,
	UI_ACTION_TX_ERROR,
	UI_ACTION_TX_RESET
} PigeonUIAction;

void pigeon_ui_init();
void pigeon_ui_destroy();
void pigeon_ui_start();
void pigeon_ui_stop();

void pigeon_ui_action(PigeonUIAction action);
void pigeon_ui_set_flash_str(const char *output, int time_seconds);
void pigeon_ui_set_display_count(int count);
bool pigeon_ui_is_reset_pressed();

#endif
