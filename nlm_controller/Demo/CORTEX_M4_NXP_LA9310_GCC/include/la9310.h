/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017-2018, 2021 NXP
 * All rights reserved.
 */

#ifndef __LA9310_H__
#define __LA9310_H__

#define PRE_SYS_FREQ     100000000
#define POST_SYS_FREQ    122880000 /*NLM final DCS frequency to switch */

/*this macro control clock switch from 100Mz to 122.88MHz.
 * When this macro is undefind VSPA, RFIC , DCS block will be disabled
 */
#define LA9310_CLOCK_SWITCH_ENABLE

#define UART_BAUDRATE                     115200
#define EARLY_UART_CLOCK_FREQUENCY        PRE_SYS_FREQ

#ifdef LA9310_CLOCK_SWITCH_ENABLE
    #define FINAL_UART_CLOCK_FREQUENCY    ( POST_SYS_FREQ * 2 ) /* Fix Me */
    #define FINAL_I2C_CLOCK_FREQUENCY     ( POST_SYS_FREQ / 2 ) /*Fix Me */
    #define PLAT_FREQ                     ( POST_SYS_FREQ * 4 )
#else
    /* This option is for debug only */
    #define FINAL_UART_CLOCK_FREQUENCY    PRE_SYS_FREQ
    #define FINAL_I2C_CLOCK_FREQUENCY     PRE_SYS_FREQ
    #define PLAT_FREQ                     ( PRE_SYS_FREQ * 4 )
#endif

#define LA9310_I2C_FREQ                   100000

#define LX9310_BOOT_SRC_SHIFT             14
#define LX9310_BOOT_SRC_MASK              3
#endif /* __LA9310_H__ */
