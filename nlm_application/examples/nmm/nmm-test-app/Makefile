# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2021 NXP

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, detect a build directory, by looking for a path with a .config
RTE_TARGET ?= $(notdir $(abspath $(dir $(firstword $(wildcard $(RTE_SDK)/*/.config)))))

include $(RTE_SDK)/mk/rte.vars.mk

# binary name
APP = nmm-test

# all source are stored in SRCS-y
SRCS-y := main.c

CFLAGS += -O3 -pthread -I$(SRCDIR)/../lib
CFLAGS += $(WERROR_FLAGS)

LDLIBS += -L$(subst nmm-test-app,lib,$(RTE_OUTPUT))/lib
LDLIBS += -lnmm -lnmmdlproc

include $(RTE_SDK)/mk/rte.extapp.mk
