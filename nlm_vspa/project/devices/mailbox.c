// =============================================================================
//! @file           mailbox.c
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
#include "vspa.h"

#define VCPU_MBOX_STATUS	0x660
#define VCPU_OUT_0_MSB		0x640
#define VCPU_OUT_0_LSB		0x644
#define VCPU_OUT_1_MSB		0x648
#define VCPU_OUT_1_LSB		0x64C
#define VCPU_IN_0_MSB		0x650
#define VCPU_IN_0_LSB		0x654
#define VCPU_IN_1_MSB		0x658
#define VCPU_IN_1_LSB		0x65C


uint32_t get_mbox_status()
{
	return __iprd(VCPU_MBOX_STATUS >> 2);
}

void mbox0_send(volatile uint32_t MSB, volatile uint32_t LSB)
{
	__ipwr(VCPU_OUT_0_MSB >> 2, MSB);
	__ipwr(VCPU_OUT_0_LSB >> 2, LSB);
}

void mbox0_get(volatile uint32_t* MSB, volatile uint32_t* LSB)
{
	*MSB = __iprd(VCPU_IN_0_MSB >> 2);
	*LSB = __iprd(VCPU_IN_0_LSB >> 2);
	__ipwr(0x8 >> 2, 0x00100000, 0x00100000);
}

void mbox1_send(volatile uint32_t MSB, volatile uint32_t LSB)
{
	__ipwr(VCPU_OUT_1_MSB >> 2, MSB);
	__ipwr(VCPU_OUT_1_LSB >> 2, LSB);
}

void mbox1_get(volatile uint32_t* MSB, volatile uint32_t* LSB)
{
	*MSB = __iprd(VCPU_IN_1_MSB >> 2);
	*LSB = __iprd(VCPU_IN_1_LSB >> 2);
	__ipwr(0x8 >> 2, 0x00200000, 0x00200000);
}
