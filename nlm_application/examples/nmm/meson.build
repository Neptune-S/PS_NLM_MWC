# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2021 NXP

# meson file, for building this example as part of a main DPDK build.
#
# To build this example as a standalone application with an already-installed
# DPDK instance, use 'make'

build = is_linux
sources = files('lib/nmm_lib.c',
	'ps_nr_pdcch_cce_to_reg_demapping',
	'lib/ps_nr_pdcch_deratematching.c',
	'lib/ps_nr_pdcch_sym_descrambler.c',
	'lib/ps_nr_pseudo_random_generator.c',
	'lib/ps_nr_qpsk_demodulation.c',
	'lib/ps_nr_pdcch_coreset_extraction.c',
	'lib/ps_nr_pdcch_channel_estimator.c',
	'lib/ps_nr_pdcch_equalizer.c',
	'lib/ps_nr_pdcch_dmrs_gen.c',
    'lib/ps_nr_parse_dci_1_0_si_sib_1.c',
    'lib/ps_nr_pdcch_crc_check.c',
    'lib/ps_nr_pdcch_polar_decoder.c',
    'lib/nr_ps_pdcch_utility_helper',
	'nmm-test-app/main.c')
includes = include_directories('lib', 'nmm-test-app')
