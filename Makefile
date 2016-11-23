FLAVOURS := default armhf

BINS := pigeond pigeon_admin

OBJS_pigeond = \
	pigeond/main.o \
	pigeond/beagle_joystick.o \
	pigeond/command_runner.o \
	pigeond/command_server.o \
	pigeond/debounce.o \
	pigeond/gpio.o \
	pigeond/long_thread.o \
	pigeond/pigeon_frame.o \
	pigeond/pigeon_transmit.o \
	pigeond/pigeon_tunnel.o \
	pigeond/util.o

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
LDFLAGS ?= -lm -lpthread -ldmtx
LDFLAGS_armhf ?= $(LDFLAGS) -L ./libs_armhf

include Makefile.base
