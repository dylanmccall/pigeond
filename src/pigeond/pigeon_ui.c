#include "pigeon_ui.h"

#include "audioMixer.h"
#include "beagle_display.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static PigeonUI *pigeon_ui;

struct _PigeonUI {
	BeagleDisplay *beagle_display;
	pthread_mutex_t ui_mutex;
	wavedata_t *beep;
	wavedata_t *slow_busy_sound;
};

void pigeon_ui_init() {
	pigeon_ui = malloc(sizeof(PigeonUI));
	memset(pigeon_ui, 0, sizeof(*pigeon_ui));
	pthread_mutex_init(&pigeon_ui->ui_mutex, NULL);
	pigeon_ui->beagle_display = beagle_display_new();
	pigeon_ui->beep = AudioMixer_waveData_new();
	pigeon_ui->slow_busy_sound = AudioMixer_waveData_new();
}

void pigeon_ui_destroy() {
	if (!pigeon_ui) {
		return;
	}

	if (pigeon_ui->beagle_display) {
		beagle_display_free(pigeon_ui->beagle_display);
	}

	if (pigeon_ui->beep) {
		AudioMixer_waveData_free(pigeon_ui->beep);
	}

	if (pigeon_ui->slow_busy_sound) {
		AudioMixer_waveData_free(pigeon_ui->slow_busy_sound);
	}

	free(pigeon_ui);
	pigeon_ui = NULL;
}

void pigeon_ui_start() {
	beagle_display_start(pigeon_ui->beagle_display);
	AudioMixer_readWaveFileIntoMemory("data/SoundEffects/beep.wav", pigeon_ui->beep);
	AudioMixer_readWaveFileIntoMemory("data/SoundEffects/testSound.wav", pigeon_ui->slow_busy_sound);
}

void pigeon_ui_stop() {
	beagle_display_stop(pigeon_ui->beagle_display);
}

void pigeon_ui_action(PigeonUIAction action) {
	switch (action) {
		case UI_ACTION_RX_BUSY: {
			pigeon_ui_set_flash_str("++", 2);
			break;
		}
		case UI_ACTION_RX_WAITING: {
			break;
		}
		case UI_ACTION_RX_SUCCESS: {
			pigeon_ui_set_flash_str("<<", 1);
			break;
		}
		case UI_ACTION_RX_ERROR: {
			pigeon_ui_set_flash_str("EE", 2);
			break;
		}
		case UI_ACTION_TX_BUSY: {
			pigeon_ui_set_flash_str("--", 2);
			break;
		}
		case UI_ACTION_TX_BUSY_SLOW: {
			pigeon_ui_set_flash_str("--", 2);
			AudioMixer_queueSound(pigeon_ui->slow_busy_sound);
			break;
		}
		case UI_ACTION_TX_WAITING: {
			pigeon_ui_set_flash_str(">>", 2);
			AudioMixer_queueSound(pigeon_ui->beep);
			break;
		}
		case UI_ACTION_TX_SUCCESS: {
			pigeon_ui_set_flash_str("GO", 1);
			AudioMixer_queueSound(pigeon_ui->beep);
			break;
		}
		case UI_ACTION_TX_ERROR: {
			pigeon_ui_set_flash_str("EE", 2);
			AudioMixer_queueSound(pigeon_ui->beep);
			break;
		}
	}
}

void pigeon_ui_set_flash_str(const char *output, int time_seconds) {
	beagle_display_set_flash(pigeon_ui->beagle_display, output, time_seconds);
}

void pigeon_ui_set_display_count(int count) {
	beagle_display_set_output_number(pigeon_ui->beagle_display, count);
}
