// =============================================================================
//! @file           phy-timer.c
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
#include "dma.h"

/**
 * @addtogroup  GROUP_PHY_TIMER
 * @{
 */

/**
 * @defgroup    GROUP_PHY_TIMER_REG Register Interface
 * @{
 */

/**
 * @defgroup    GROUP_PHY_TIMER_REG_ADDR    Register Map
 * @{
 */

#define PHY_TMR_CTRL    (LA9310_CCSR_BASE_PHY_TIMER + 0x00)    /**< PHY Timer control register address. */
#define PHY_TMR_C0SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x04)    /**< PHY Timer comparator # 0 status and control register address.  */
#define PHY_TMR_C0V     (LA9310_CCSR_BASE_PHY_TIMER + 0x08)    /**< PHY Timer comparator # 0 value register address.               */
#define PHY_TMR_C1SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x0C)    /**< PHY Timer comparator # 1 status and control register address.  */
#define PHY_TMR_C1V     (LA9310_CCSR_BASE_PHY_TIMER + 0x10)    /**< PHY Timer comparator # 1 value register address.               */
#define PHY_TMR_C2SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x14)    /**< PHY Timer comparator # 2 status and control register address.  */
#define PHY_TMR_C2V     (LA9310_CCSR_BASE_PHY_TIMER + 0x18)    /**< PHY Timer comparator # 2 value register address.               */
#define PHY_TMR_C3SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x1C)    /**< PHY Timer comparator # 3 status and control register address.  */
#define PHY_TMR_C3V     (LA9310_CCSR_BASE_PHY_TIMER + 0x20)    /**< PHY Timer comparator # 3 value register address.               */
#define PHY_TMR_C4SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x24)    /**< PHY Timer comparator # 4 status and control register address.  */
#define PHY_TMR_C4V     (LA9310_CCSR_BASE_PHY_TIMER + 0x28)    /**< PHY Timer comparator # 4 value register address.               */
#define PHY_TMR_C5SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x2C)    /**< PHY Timer comparator # 5 status and control register address.  */
#define PHY_TMR_C5V     (LA9310_CCSR_BASE_PHY_TIMER + 0x30)    /**< PHY Timer comparator # 5 value register address.               */
#define PHY_TMR_C6SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x34)    /**< PHY Timer comparator # 6 status and control register address.  */
#define PHY_TMR_C6V     (LA9310_CCSR_BASE_PHY_TIMER + 0x38)    /**< PHY Timer comparator # 6 value register address.               */
#define PHY_TMR_C7SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x3C)    /**< PHY Timer comparator # 7 status and control register address.  */
#define PHY_TMR_C7V     (LA9310_CCSR_BASE_PHY_TIMER + 0x40)    /**< PHY Timer comparator # 7 value register address.               */
#define PHY_TMR_C8SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x44)    /**< PHY Timer comparator # 8 status and control register address.  */
#define PHY_TMR_C8V     (LA9310_CCSR_BASE_PHY_TIMER + 0x48)    /**< PHY Timer comparator # 8 value register address.               */
#define PHY_TMR_C9SC    (LA9310_CCSR_BASE_PHY_TIMER + 0x4C)    /**< PHY Timer comparator # 9 status and control register address.  */
#define PHY_TMR_C9V     (LA9310_CCSR_BASE_PHY_TIMER + 0x50)    /**< PHY Timer comparator # 9 value register address.               */
#define PHY_TMR_C10SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x54)    /**< PHY Timer comparator #10 status and control register address.  */
#define PHY_TMR_C10V    (LA9310_CCSR_BASE_PHY_TIMER + 0x58)    /**< PHY Timer comparator #10 value register address.               */
#define PHY_TMR_C11SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x5C)    /**< PHY Timer comparator #11 status and control register address.  */
#define PHY_TMR_C11V    (LA9310_CCSR_BASE_PHY_TIMER + 0x60)    /**< PHY Timer comparator #11 value register address.               */
#define PHY_TMR_C12SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x64)    /**< PHY Timer comparator #12 status and control register address.  */
#define PHY_TMR_C12V    (LA9310_CCSR_BASE_PHY_TIMER + 0x68)    /**< PHY Timer comparator #12 value register address.               */
#define PHY_TMR_C13SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x6C)    /**< PHY Timer comparator #13 status and control register address.  */
#define PHY_TMR_C13V    (LA9310_CCSR_BASE_PHY_TIMER + 0x70)    /**< PHY Timer comparator #13 value register address.               */
#define PHY_TMR_C14SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x74)    /**< PHY Timer comparator #14 status and control register address.  */
#define PHY_TMR_C14V    (LA9310_CCSR_BASE_PHY_TIMER + 0x78)    /**< PHY Timer comparator #14 value register address.               */
#define PHY_TMR_C15SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x7C)    /**< PHY Timer comparator #15 status and control register address.  */
#define PHY_TMR_C15V    (LA9310_CCSR_BASE_PHY_TIMER + 0x80)    /**< PHY Timer comparator #15 value register address.               */
#define PHY_TMR_C16SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x84)    /**< PHY Timer comparator #16 status and control register address.  */
#define PHY_TMR_C16V    (LA9310_CCSR_BASE_PHY_TIMER + 0x88)    /**< PHY Timer comparator #16 value register address.               */
#define PHY_TMR_C17SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x8C)    /**< PHY Timer comparator #17 status and control register address.  */
#define PHY_TMR_C17V    (LA9310_CCSR_BASE_PHY_TIMER + 0x90)    /**< PHY Timer comparator #17 value register address.               */
#define PHY_TMR_C18SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x94)    /**< PHY Timer comparator #18 status and control register address.  */
#define PHY_TMR_C18V    (LA9310_CCSR_BASE_PHY_TIMER + 0x98)    /**< PHY Timer comparator #18 value register address.               */
#define PHY_TMR_C19SC   (LA9310_CCSR_BASE_PHY_TIMER + 0x9C)    /**< PHY Timer comparator #19 status and control register address.  */
#define PHY_TMR_C19V    (LA9310_CCSR_BASE_PHY_TIMER + 0xA0)    /**< PHY Timer comparator #19 value register address.               */
#define PHY_TMR_C20SC   (LA9310_CCSR_BASE_PHY_TIMER + 0xA4)    /**< PHY Timer comparator #20 status and control register address.  */
#define PHY_TMR_C20V    (LA9310_CCSR_BASE_PHY_TIMER + 0xA8)    /**< PHY Timer comparator #20 value register address.               */
#define PHY_TMR_C21SC   (LA9310_CCSR_BASE_PHY_TIMER + 0xAC)    /**< PHY Timer comparator #21 status and control register address.  */
#define PHY_TMR_C21V    (LA9310_CCSR_BASE_PHY_TIMER + 0xB0)    /**< PHY Timer comparator #21 value register address.               */
#define PHY_TMR_C22SC   (LA9310_CCSR_BASE_PHY_TIMER + 0xB4)    /**< PHY Timer comparator #22 status and control register address.  */
#define PHY_TMR_C22V    (LA9310_CCSR_BASE_PHY_TIMER + 0xB8)    /**< PHY Timer comparator #22 value register address.               */

/** @} */

/**
 * @defgroup    GROUP_PHY_TIMER_REG_DEF     Register Definitions
 * @{
 */

#define PHY_TMR_CTRL_SHIFT_EN           0
#define PHY_TMR_CTRL_SHIFT_SRST         4
#define PHY_TMR_CTRL_SHIFT_DIV          8

#define PHY_TMR_CTRL_FIELD_DIV          (0x3F << PHY_TMR_CTRL_SHIFT_DIV )

#define PHY_TMR_CTRL_EN                 (0x01 << PHY_TMR_CTRL_SHIFT_EN  )
#define PHY_TMR_CTRL_SRST               (0x01 << PHY_TMR_CTRL_SHIFT_SRST)

#define PHY_TMR_CSC_SHIFT_CMP_TRIG      0
#define PHY_TMR_CSC_SHIFT_DIR_TRIG      2
#define PHY_TMR_CSC_SHIFT_CTE           4
#define PHY_TMR_CSC_SHIFT_CAP           5
#define PHY_TMR_CSC_SHIFT_CMPE          6
#define PHY_TMR_CSC_SHIFT_CIF           7
#define PHY_TMR_CSC_SHIFT_CMPCAP        8
#define PHY_TMR_CSC_SHIFT_TVAL          31

#define PHY_TMR_CSC_FIELD_CAP			(0x1 << PHY_TMR_CSC_SHIFT_CAP)
#define PHY_TMR_CSC_FIELD_CTE			(0x1 << PHY_TMR_CSC_SHIFT_CTE)
#define PHY_TMR_CSC_FIELD_CIF			(0x1 << PHY_TMR_CSC_SHIFT_CIF)

#define PHY_TMR_CSC_FIELD_CMP_TRIG      (0x3 << PHY_TMR_CSC_SHIFT_CMP_TRIG)
#define PHY_TMR_CSC_FIELD_DIR_TRIG      (0x3 << PHY_TMR_CSC_SHIFT_DIR_TRIG)

#define PHY_TMR_CSC_CMP_TRIG_NO_CHANGE  (0x0 << PHY_TMR_CSC_SHIFT_CMP_TRIG)
#define PHY_TMR_CSC_CMP_TRIG_FORCE0     (0x1 << PHY_TMR_CSC_SHIFT_CMP_TRIG)
#define PHY_TMR_CSC_CMP_TRIG_FORCE1     (0x2 << PHY_TMR_CSC_SHIFT_CMP_TRIG)
#define PHY_TMR_CSC_CMP_TRIG_INVERT     (0x3 << PHY_TMR_CSC_SHIFT_CMP_TRIG)

#define PHY_TMR_CSC_DIR_TRIG_NO_CHANGE  (0x0 << PHY_TMR_CSC_SHIFT_DIR_TRIG)
#define PHY_TMR_CSC_DIR_TRIG_FORCE0     (0x1 << PHY_TMR_CSC_SHIFT_DIR_TRIG)
#define PHY_TMR_CSC_DIR_TRIG_FORCE1     (0x2 << PHY_TMR_CSC_SHIFT_DIR_TRIG)
#define PHY_TMR_CSC_DIR_TRIG_INVERT     (0x3 << PHY_TMR_CSC_SHIFT_DIR_TRIG)

#define PHY_TMR_CSC_CMPE                (0x1 << PHY_TMR_CSC_SHIFT_CMPE    )

/** @} */

/**
 * @defgroup    GROUP_PHY_TIMER_REG_TYPE    Register Data Types
 * @{
 */

/**
 * @brief       PHY Timer comparator registers type.
 */
struct phy_tmr_cmp_t {
	uint32_t sc;            /**< Comparator status and control register. */
	uint32_t v;             /**< Comparator value register. */
};

/**
 * @brief       PHY Timer registers type.
 */
struct phy_tmr_regs_t {
	uint32_t control;                       /**< PHY Timer control register.        */
	struct phy_tmr_cmp_t comparator[23];    /**< PHY Timer comparators registers.   */
};

/**
 * @brief       PHY Timer comparator enumerations.
 */
enum phy_tmr_e {
	PHY_TMR_AXIQ_TX,    /** AXIQ transmit channel comparator.   */
	PHY_TMR_AXIQ_RXA,   /** AXIQ receive channel A comparator.  */
	PHY_TMR_AXIQ_RXB,   /** AXIQ receive channel B comparator.  */
	PHY_TMR_AXIQ_RXC,   /** AXIQ receive channel C comparator.  */
	PHY_TMR_AXIQ_RXD,   /** AXIQ receive channel D comparator.  */
	PHY_TMR_COUNT       /** Number of comparators.              */
};

/** @} */


#define PHYTMR_DMA_CTRL (DMA_DI_32 | DMA_PHY_TIMER)

// PHY Timer configuration
__attribute__((aligned(8))) static uint32_t phy_tmr_sc_data[8];
__attribute__((aligned(8))) static uint32_t const phy_tmr_sc_addr[] = {
		/* PHY_TMR_AXIQ_TX : */  PHY_TMR_C11SC,
		/* PHY_TMR_AXIQ_RXA: */  PHY_TMR_C1SC,
		/* PHY_TMR_AXIQ_RXB: */  PHY_TMR_C2SC,
		/* PHY_TMR_AXIQ_RXC: */  PHY_TMR_C3SC,
		/* PHY_TMR_AXIQ_RXD: */  PHY_TMR_C4SC,
};

__attribute__((aligned(8))) static uint32_t const phy_tmr_v_addr[] = {
		/* PHY_TMR_AXIQ_TX : */  PHY_TMR_C11V,
		/* PHY_TMR_AXIQ_RXA: */  PHY_TMR_C1V,
		/* PHY_TMR_AXIQ_RXB: */  PHY_TMR_C2V,
		/* PHY_TMR_AXIQ_RXC: */  PHY_TMR_C3V,
		/* PHY_TMR_AXIQ_RXD: */  PHY_TMR_C4V,
};


__attribute__((aligned(8))) static uint32_t const phy_tmr0_sc_addr[] = {
		/* PHY_TMR_AXIQ_RXB : */  PHY_TMR_C2SC,
};


__attribute__((aligned(8))) static uint32_t const phy_tmr_ctrl_addr = PHY_TMR_CTRL;

__attribute__((aligned(8))) static uint32_t ctrl;

void phy_tmr_enable(void)
{
	ctrl = PHY_TMR_CTRL_EN;
	dma_ch_init(DMA_PHY_TIMER);

	PHY_TMR_PRG((uint32_t)&phy_tmr_ctrl_addr, (uint32_t)&ctrl, 1);
	dma_ch_busy_wait(DMA_PHY_TIMER);
}

void phy_tmr_reset(void)
{
	ctrl = PHY_TMR_CTRL_SRST;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)&phy_tmr_ctrl_addr, (uint32_t)&ctrl, 1);
	dma_ch_busy_wait(DMA_PHY_TIMER);

	ctrl = 0;
	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)&phy_tmr_ctrl_addr, (uint32_t)&ctrl, 1);
	dma_ch_busy_wait(DMA_PHY_TIMER);

	phy_tmr_sc_data[PHY_TMR_AXIQ_TX ] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXA] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXB] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXC] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXD] = PHY_TMR_CSC_DIR_TRIG_FORCE1;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)&phy_tmr_sc_addr, (uint32_t)&phy_tmr_sc_data, 6);
	dma_ch_busy_wait(DMA_PHY_TIMER);

	phy_tmr_sc_data[PHY_TMR_AXIQ_TX ] = PHY_TMR_CSC_DIR_TRIG_FORCE0;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXA] = PHY_TMR_CSC_DIR_TRIG_FORCE0;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXB] = PHY_TMR_CSC_DIR_TRIG_FORCE0;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXC] = PHY_TMR_CSC_DIR_TRIG_FORCE0;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXD] = PHY_TMR_CSC_DIR_TRIG_FORCE0;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)&phy_tmr_sc_addr, (uint32_t)&phy_tmr_sc_data, 6);
	dma_ch_busy_wait(DMA_PHY_TIMER);
}

void phy_tmr_configure(void)
{
	
	/* Write ALWAYS ON configuration to data table: trigger */
	phy_tmr_sc_data[0] = PHY_TMR_CSC_DIR_TRIG_FORCE1 | PHY_TMR_CSC_CMPE;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)phy_tmr0_sc_addr, (uint32_t)&phy_tmr_sc_data, 1);
	dma_ch_busy_wait(DMA_PHY_TIMER);

#if 0
	/* Write ALWAYS ON configuration to data table: trigger */
	phy_tmr_sc_data[PHY_TMR_AXIQ_TX ] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXA] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXB] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXC] = PHY_TMR_CSC_DIR_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXD] = PHY_TMR_CSC_DIR_TRIG_FORCE1;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)phy_tmr_sc_addr, (uint32_t)&phy_tmr_sc_data, 6);
	dma_ch_busy_wait(DMA_PHY_TIMER);
#endif

#if 0
	/* Write LTE SYMBOL configuration to data table: trigger */
	phy_tmr_sc_data[PHY_TMR_AXIQ_TX ] = PHY_TMR_CSC_DIR_TRIG_NO_CHANGE | PHY_TMR_CSC_CMP_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXA] = PHY_TMR_CSC_DIR_TRIG_NO_CHANGE | PHY_TMR_CSC_CMP_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXB] = PHY_TMR_CSC_DIR_TRIG_NO_CHANGE | PHY_TMR_CSC_CMP_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXC] = PHY_TMR_CSC_DIR_TRIG_NO_CHANGE | PHY_TMR_CSC_CMP_TRIG_FORCE1;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXD] = PHY_TMR_CSC_DIR_TRIG_NO_CHANGE | PHY_TMR_CSC_CMP_TRIG_FORCE1;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)phy_tmr_sc_addr, (uint32_t)&phy_tmr_sc_data, 6);
	dma_ch_busy_wait(DMA_PHY_TIMER);

	/* Write configuration to data table: */
	/* 71.42us * 61.44MHz */
	phy_tmr_sc_data[PHY_TMR_AXIQ_TX ] = 4388;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXA] = 4388;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXB] = 4388;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXC] = 4388;
	phy_tmr_sc_data[PHY_TMR_AXIQ_RXD] = 4388;

	/* Transfer data table: */
	PHY_TMR_PRG((uint32_t)phy_tmr_v_addr, (uint32_t)&phy_tmr_sc_data, 6);
	dma_ch_busy_wait(DMA_PHY_TIMER);
#endif
}

