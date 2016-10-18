FLAVOURS := default armhf

BINS := pigeond

OBJS_pigeond = \
	pigeond/main.o

SRC ?= ./src
OUT ?= ./out
BUILD ?= ./build

EXPORT_DIR_armhf ?= $(HOME)/cmpt433/public/myApps
SEND_SSH_armhf ?= root@dmccall-beagle.local

MAKE ?= make
INSTALL ?= install

CC ?= gcc
CC_armhf ?= arm-linux-gnueabihf-gcc
CFLAGS ?= -g -Wall -Werror -std=c99 -D _POSIX_C_SOURCE=200809L
LDFLAGS ?= -lm -lpthread

.PHONY: default
default: all

.PHONY: all
all: build.all

.PHONY: build
build: build.all

.PHONY: build.all

.PHONY: clean
clean:
	-rmdir $(OUT)
	-rmdir $(BUILD)

.PHONY: export

.PHONY: send

$(EXPORT_DIR):
	mkdir -p $@

define add_flavour_bin_build

LDFLAGS_$(bin) ?= $(LDFLAGS)

_F_BINS_$(flavour)_$(bin) := $(OUT)/$(flavour)/$(bin)
_F_OBJS_$(flavour)_$(bin) := $(addprefix $(BUILD)/$(flavour)/,$(OBJS_$(bin)))
_F_DEPS_$(flavour)_$(bin) := $(addprefix $(BUILD)/$(flavour)/,$(OBJS_$(bin):.o=.d))

-include $$(_F_DEPS_$(flavour)_$(bin))

build.$(flavour): $$(_F_BINS_$(flavour)_$(bin))

.PHONY: build.$(flavour).$(bin)
build.$(flavour): build.$(flavour).$(bin)
build.$(flavour).$(bin): $$(_F_BINS_$(flavour)_$(bin))

.PHONY: build.all.$(bin)
build.all.$(bin): build.$(flavour).$(bin)

$$(_F_OBJS_$(flavour)_$(bin)): | $(BUILD)/$(flavour)
$$(_F_BINS_$(flavour)_$(bin)): | $(OUT)/$(flavour)

$$(_F_BINS_$(flavour)_$(bin)): $$(_F_OBJS_$(flavour)_$(bin))
$$(_F_BINS_$(flavour)_$(bin)):
	mkdir -p $$(@D)
	$$(CC_$(flavour)) $$+ -o $$@ $$(LDFLAGS_$(bin))

ifdef EXPORT_DIR_$(flavour)
.PHONY: export.$(flavour).$(bin)
export.$(flavour): export.$(flavour).$(bin)
export.$(flavour).$(bin): $(EXPORT_DIR_$(flavour))/$(bin)
$(EXPORT_DIR_$(flavour))/$(bin): | $(EXPORT_DIR_$(flavour))
$(EXPORT_DIR_$(flavour))/$(bin): $$(_F_BINS_$(flavour)_$(bin))
	install $$< $$(@D)
endif

ifdef SEND_SSH_$(flavour)
.PHONY: send.$(flavour).$(bin)
send.$(flavour): send.$(flavour).$(bin)
send.$(flavour).$(bin): $$(_F_BINS_$(flavour)_$(bin))
	scp $$< $(SEND_SSH_$(flavour)):
endif

.PHONY: clean.$(flavour).$(bin)
clean.$(flavour): clean.$(flavour).$(bin)
clean.$(flavour).$(bin):
	rm -f $$(_F_BINS_$(flavour)_$(bin))
	rm -f $$(_F_OBJS_$(flavour)_$(bin))
	rm -f $$(_F_DEPS_$(flavour)_$(bin))

endef


define add_flavour_build
# add a rule for each build flavour in FLAVOURS

BINS_$(flavour) ?= $(BINS)
CC_$(flavour) ?= $(CC)
CFLAGS_$(flavour) ?= $(CFLAGS)
EXPORT_DIR_$(flavour) ?= $(EXPORT_DIR)

_F_BUILD_$(flavour) := $(BUILD)/$(flavour)
_F_OUT_$(flavour) := $(OUT)/$(flavour)

$$(foreach bin, $$(BINS_$(flavour)), $$(eval $$(call add_flavour_bin_build, $(flavour), $$(bin))))

.PHONY: build.$(flavour)
build.all: build.$(flavour)

$$(_F_BUILD_$(flavour)) $$(_F_OUT_$(flavour)):
	mkdir -p $$@

$$(_F_BUILD_$(flavour))/%.o: $(SRC)/%.c
	mkdir -p $$(@D)
	$$(CC_$(flavour)) $$(CFLAGS_$(flavour)) -c $$< -o $$@
	$$(CC_$(flavour)) -MM $$(CFLAGS_$(flavour)) -c $$< -MT $$@ -MF $$(@:.o=.d)

ifdef EXPORT_DIR_$(flavour)
.PHONY: export.$(flavour)
export: export.$(flavour)
$(EXPORT_DIR_$(flavour)):
	mkdir -p $$@
endif

ifdef SEND_SSH_$(flavour)
.PHONY: send.$(flavour)
send: send.$(flavour)
endif

.PHONY: clean.$(flavour)
clean: clean.$(flavour)
clean.$(flavour):
	-find $$(_F_BUILD_$(flavour)) -type d -empty -delete
	-find $$(_F_OUT_$(flavour)) -type d -empty -delete

endef
$(foreach flavour, $(FLAVOURS), $(eval $(call add_flavour_build, $(flavour))))
