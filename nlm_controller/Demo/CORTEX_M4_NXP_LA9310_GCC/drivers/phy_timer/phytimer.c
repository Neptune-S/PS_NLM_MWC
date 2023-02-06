/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2021 NXP
 * All rights reserved.
 */

#include <phytimer.h>
#include "cmd_ctrl_vspa_intfce.h"

#define PHY_TIMER_DIVISOR_MASK 0x3F

struct xPhyTimerRegs *regs = (struct xPhyTimerRegs *)(PHY_TIMER_BASE_ADDRESS);

static uint32_t ulNextPPSOUT;
static uint32_t prvPhyTimerComparatorGetConfig(uint8_t ucComparator)
{
    return IN_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs);
}

void vPhyTimerReset()
{
    OUT_32(&regs->ulTmPhyTmrCtrl, 0);
    OUT_32(&regs->ulTmPhyTmrCtrl, BIT(4));
    OUT_32(&regs->ulTmPhyTmrCtrl, 0);
}

void vPhyTimerEnable(uint8_t ucClockDiv)
{
    OUT_32(&regs->ulTmPhyTmrCtrl,
           (uint32_t)((ucClockDiv & PHY_TIMER_DIVISOR_MASK) << 8));
    OUT_32(&regs->ulTmPhyTmrCtrl,
           (uint32_t)((ucClockDiv & PHY_TIMER_DIVISOR_MASK) << 8) | BIT(4));
    OUT_32(&regs->ulTmPhyTmrCtrl,
           (uint32_t)((ucClockDiv & PHY_TIMER_DIVISOR_MASK) << 8));
    OUT_32(&(regs->ulTmPhyTmrCtrl),
           (uint32_t)((ucClockDiv & PHY_TIMER_DIVISOR_MASK) << 8) | 1);
}

void vPhyTimerDisable()
{
    OUT_32(&regs->ulTmPhyTmrCtrl, 0);
}

uint32_t ulPhyTimerCapture(uint8_t ucComparator)
{
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs,
           (uint32_t)PHY_TIMER_COMPARATOR_CAPTURE |
               (ePhyTimerComparatorNoChange << 2) |
               ePhyTimerComparatorNoChange);

    return IN_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCnv);
}

void ulPhyTimerCaptureReset(uint8_t ucComparator)
{
	    regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs = 
		            (regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs & ~(1 << 5));           

}

void vPhyTimerComparatorDisable(uint8_t ucComparator)
{
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs,
           PHY_TIMER_COMPARATOR_DISABLE);
}

void vPhyTimerComparatorForce(uint8_t ucComparator,
                              enum ePhyTimerComparatorTrigger eValue)
{
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs,
           PHY_TIMER_COMPARATOR_DISABLE);
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs,
           (uint32_t)eValue << 2);
}

void vPhyTimerComparatorConfig(uint8_t ucComparator,
                               uint8_t ucFlags,
                               enum ePhyTimerComparatorTrigger eCmpTrig,
                               uint32_t ulTriggerValue)
{
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs, PHY_TIMER_COMPARATOR_DISABLE);
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs, (uint32_t)ucFlags | eCmpTrig);
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCnv, ulTriggerValue);
}

uint32_t ulPhyTimerComparatorRead(uint8_t ucComparator)
{
    return IN_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCnv);
}

uint32_t ulPhyTimerComparatorGetStatus(uint8_t ucComparator)
{
    return prvPhyTimerComparatorGetConfig(ucComparator) &
           PHY_TIMER_COMPARATOR_STATUS_MASK;
}

void vPhyTimerUpdateComparator(uint8_t ucComparator,
                               uint32_t ulTriggerValue)
{
    uint32_t *addr = &regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCncrs;
    uint32_t ulTmPhyTmrCncrs = IN_32(addr);

    OUT_32(addr, ulTmPhyTmrCncrs | PHY_TIMER_COMPARATOR_DISABLE);
    OUT_32(&regs->xTmPhyTmrN[ucComparator].ulTmPhyTmrCnv, ulTriggerValue);
}

void vPhyTimerPPSOUTConfig()
{
    NVIC_SetPriority(IRQ_PPS_OUT, 1);
    NVIC_EnableIRQ(IRQ_PPS_OUT);

    ulNextPPSOUT = ulPhyTimerCapture(PHY_TIMER_COMP_PPS_OUT);
    ulNextPPSOUT += MSECONDS_TO_PHY_TIMER_COUNT(PHY_TIMER_PPS_OUT_PULSE_DELAY);

    vPhyTimerComparatorConfig(PHY_TIMER_COMP_PPS_OUT,
                              PHY_TIMER_COMPARATOR_CLEAR_INT | PHY_TIMER_COMPARATOR_CROSS_TRIG,
                              ePhyTimerComparatorOutToggle,
                              ulNextPPSOUT);
}

void vPhyTimerPPSOUTHandler()
{
    NVIC_ClearPendingIRQ(IRQ_PPS_OUT);

    ulNextPPSOUT += MSECONDS_TO_PHY_TIMER_COUNT(PHY_TIMER_PPS_OUT_PULSE_DELAY);

    vPhyTimerComparatorConfig(PHY_TIMER_COMP_PPS_OUT,
                              PHY_TIMER_COMPARATOR_CLEAR_INT | PHY_TIMER_COMPARATOR_CROSS_TRIG,
                              ePhyTimerComparatorOutToggle,
                              ulNextPPSOUT);
}


static uint32_t pps_lvl_toggle = 0;

void vPhyTimerVspaGoComp0(uint32_t goCount)
{   
    vPhyTimerComparatorConfig(PHY_TIMER_COMP_VSPA_GO_0,
                              PHY_TIMER_COMPARATOR_CLEAR_INT,
                              ePhyTimerComparatorOut1,
                              goCount);                                                               
}

void vPhyTimerPPSOUTConfigGPSlike()
{
    NVIC_SetPriority(IRQ_PPS_OUT, 1);
    NVIC_EnableIRQ(IRQ_PPS_OUT);

    pps_lvl_toggle = 0;

    ulNextPPSOUT = ulPhyTimerCapture(PHY_TIMER_COMP_PPS_OUT);
    ulNextPPSOUT += MSECONDS_TO_PHY_TIMER_COUNT(1000);
    
    vPhyTimerComparatorConfig(PHY_TIMER_COMP_PPS_OUT,
                              PHY_TIMER_COMPARATOR_CLEAR_INT | PHY_TIMER_COMPARATOR_CROSS_TRIG,
                              ePhyTimerComparatorOut1,
                              ulNextPPSOUT);
}

void vPhyTimerPPSOUTHandlerGPSlike()
{
    enum ePhyTimerComparatorTrigger output_level = ePhyTimerComparatorOut1;
    NVIC_ClearPendingIRQ(IRQ_PPS_OUT);

    pps_lvl_toggle = (pps_lvl_toggle + 1);

    if(pps_lvl_toggle == 50)
    {
       pps_lvl_toggle = 0;
    }
       
    ulNextPPSOUT += MSECONDS_TO_PHY_TIMER_COUNT(1000) ;

    if (pps_lvl_toggle < 2)
    {
        output_level = ePhyTimerComparatorOut1;
    }
    else
    {
        output_level = ePhyTimerComparatorOut0;
    }
    
    vPhyTimerComparatorConfig(PHY_TIMER_COMP_PPS_OUT,
                              PHY_TIMER_COMPARATOR_CLEAR_INT | PHY_TIMER_COMPARATOR_CROSS_TRIG,
                              output_level,
                              ulNextPPSOUT);
}

void vPhyTimerPPSOUTAdjustMinor(int32_t offset)
{
    /* Minor adjustment, fix on next interrupt */
    ulNextPPSOUT += offset;
}

void vPhyTimerPPSOUTAdjustMajor(uint32_t timestamp, uint32_t offset)
{
    /* Major adjustment, reconfigure phy timer */

    /* offset is always positive in this case */
    ulNextPPSOUT = timestamp + offset;
    vPhyTimerComparatorConfig(PHY_TIMER_COMP_PPS_OUT,
                              PHY_TIMER_COMPARATOR_CLEAR_INT | PHY_TIMER_COMPARATOR_CROSS_TRIG,
                              ePhyTimerComparatorOut1,
                              ulNextPPSOUT);
    pps_lvl_toggle = 0;
}

void vPhyTimerPPSINEnable()
{
    NVIC_SetPriority(IRQ_PPS_IN, 1);
    NVIC_EnableIRQ(IRQ_PPS_IN);
}

void vPhyTimerPPSINDisable()
{
    NVIC_DisableIRQ(IRQ_PPS_IN);
}

void vPhyTimerPPSINHandler()
{
    PRINTF("%x\r\n", ulPhyTimerComparatorRead(PHY_TIMER_COMP_PPS_IN));
    NVIC_ClearPendingIRQ(IRQ_PPS_IN);
}

uint32_t ulPhyTimerDiffToUS(uint32_t ulOlderTimestamp, uint32_t ulNewerTimestamp)
{
    uint32_t ulDiff;

    if (ulNewerTimestamp > ulOlderTimestamp)
    {
        ulDiff = ulNewerTimestamp - ulOlderTimestamp;
    }
    else
    {
        ulDiff = PHY_TMR_FREQ - ulOlderTimestamp + ulNewerTimestamp;
    }

    /*
     * Not considering 0.44 in calculations to avoid floating point calc
     * It is around 0.7% error
     * */
    return ulDiff / 61;
}
