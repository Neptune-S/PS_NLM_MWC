// =============================================================================
//! @file           all_drivers.h
//! @copyright      Copyright 2021 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
#ifndef ALL_DRIVERS_H_
#define ALL_DRIVERS_H_

#include <stdint.h>
#include <size_t.h>

#define SW_VERSION_REG		0x4
#define SW_VERSION(version_major, version_minor) __ipwr(SW_VERSION_REG >> 2 , version_major , version_minor)

/* MAILBOX */

extern uint32_t get_mbox_status();
extern void 	mbox0_send(volatile uint32_t MSB, volatile uint32_t LSB);
extern void 	mbox0_get(volatile uint32_t* MSB, volatile uint32_t* LSB);
extern void 	mbox1_send(volatile uint32_t MSB, volatile uint32_t LSB);
extern void  	mbox1_get(volatile uint32_t* MSB, volatile uint32_t* LSB);


/* PHY TIMER */
#define READ_PHY_TIMER_VALUE (__iprd((0x500+4*4) >> 2)) //gp_in_regs[159:128] <= phytmr_cval

extern void phy_tmr_enable(void);
extern void phy_tmr_configure(void);
extern void phy_tmr_reset(void);
extern void phy_tmr_symbol_inc(void);
extern void phy_tmr_get_time(void);




#endif /* ALL_DRIVERS_H_ */

