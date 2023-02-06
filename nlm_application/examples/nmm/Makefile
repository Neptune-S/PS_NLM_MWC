# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2021 NXP

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, detect a build directory, by looking for a path with a .config
RTE_TARGET ?= $(notdir $(abspath $(dir $(firstword $(wildcard $(RTE_SDK)/*/.config)))))

include $(RTE_SDK)/mk/rte.vars.mk

ifneq ($(CONFIG_RTE_EXEC_ENV_LINUX),y)
$(info This application can only operate in a linux environment, \
please change the definition of the RTE_TARGET environment variable)
else

DIRS-y += lib nmm-test-app
endif

DEPDIRS-nmm-test-app := lib

include $(RTE_SDK)/mk/rte.extsubdir.mk
