# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2018-2020 NXP

if not is_linux
	build = false
	reason = 'only supported on linux'
endif

deps += ['bus_vdev', 'bus_dpaa', 'security']
sources = files('caam_jr_capabilities.c',
		'caam_jr_hw.c',
		'caam_jr_uio.c',
		'caam_jr.c')

allow_experimental_apis = true

includes += include_directories('../../bus/dpaa/include/')
includes += include_directories('../../common/dpaax/')
includes += include_directories('../../common/dpaax/caamflib/')
