FLAVOURS := default armhf

BINS := pigeond pigeon_admin

OBJS_pigeond = \
	pigeond/main.o \
	pigeond/audioMixer.o \
	pigeond/base64.o \
	pigeond/beagle_controls.o \
	pigeond/beagle_display.o \
	pigeond/beagle_joystick.o \
	pigeond/command_runner.o \
	pigeond/command_server.o \
	pigeond/debounce.o \
	pigeond/fsd_char.o \
	pigeond/gpio.o \
	pigeond/linkmods/linkmod_console.o \
	pigeond/linkmods/linkmod_files.o \
	pigeond/linkmods/linkmod_null.o \
	pigeond/linkmods/linkmod_printer.o \
	pigeond/long_thread.o \
	pigeond/pigeon_frame.o \
	pigeond/pigeon_frame_pipe.o \
	pigeond/pigeon_link.o \
	pigeond/pigeon_linkmod.o \
	pigeond/pigeon_tunnel.o \
	pigeond/pigeon_ui.o \
	pigeond/pointer_fifo.o \
	pigeond/printer.o \
	pigeond/util.o

DATA_DIRS_pigeond = \
    pigeond-data

DATA_DIRS_pigeon_admin = \
	pigeon-admin

SRC ?= ./src
DATA ?= ./data
OUT ?= ./out
BUILD ?= ./build

EXPORT_DIR_armhf ?= $(HOME)/cmpt433/public/myApps
SEND_SSH_armhf ?= root@dmccall-beagle.local

MAKE ?= make
INSTALL ?= install
RSYNC ?= rsync --partial --progress --recursive --links

CC ?= gcc
CC_armhf ?= arm-linux-gnueabihf-gcc
CFLAGS ?= -g -Wall -Werror -std=c99 -D _POSIX_C_SOURCE=200809L
CFLAGS_armhf ?= $(CFLAGS) -I ./includes_armhf
LDFLAGS ?= -lm -lpthread -lqrencode -lasound
LDFLAGS_armhf ?= $(LDFLAGS) -L ./libs_armhf

include Makefile.base
