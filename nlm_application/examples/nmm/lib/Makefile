# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2021-2022 NXP

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, detect a build directory, by looking for a path with a .config
RTE_TARGET ?= $(notdir $(abspath $(dir $(firstword $(wildcard $(RTE_SDK)/*/.config)))))

include $(RTE_SDK)/mk/rte.vars.mk

ifneq ($(CONFIG_RTE_EXEC_ENV_LINUX),y)
$(error This application can only operate in a linux environment, \
please change the definition of the RTE_TARGET environment variable)
endif

# library name
LIB = libnmm.a

LIBABIVER := 1

# all source are stored in SRC-Y
SRCS-y += nmm_lib.c
SRCS-y += ps_nr_pdcch_cce_to_reg_demapping.c
SRCS-y += ps_nr_pdcch_deratematching.c
SRCS-y += ps_nr_pseudo_random_generator.c
SRCS-y += ps_nr_pdcch_sym_descrambler.c
SRCS-y += ps_nr_qpsk_demodulation.c
SRCS-y += ps_nr_pdcch_polar_decoder.c
SRCS-y += ps_nr_pdcch_coreset_extraction.c
SRCS-y += ps_nr_pdcch_channel_estimator.c
SRCS-y += ps_nr_pdcch_equalizer.c
SRCS-y += ps_nr_pdcch_dmrs_gen.c
SRCS-y += ps_nr_parse_dci_1_0_si_sib_1.c
SRCS-y += ps_nr_pdcch_crc_check.c
SRCS-y += nr_ps_pdcch_utility_helper.c
SRCS-y += mjson.c


CFLAGS += -DALLOW_EXPERIMENTAL_API
CFLAGS += -Wall
#CFLAGS += -DNMM_LIB_DEBUG
CFLAGS += -DNMM_LIB_TIMER_RESET

include $(RTE_SDK)/mk/rte.extlib.mk
