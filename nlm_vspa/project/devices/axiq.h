// =============================================================================
//! @file           axiq.h
//! @copyright      Copyright 2017 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
#ifndef __AXIQ_H__
#define __AXIQ_H__

#include "vspa.h"

/**
 * @addtogroup  GROUP_AXIQ
 * @{
 */

/**
 * @defgroup    GROUP_AXIQ_CFG  Configuration Interface
 * @{
 */



#define AXIQ_CR_ENABLE              (0x1 << 0)
#define AXIQ_CR_THRESH              (0x3 << 1)
#define AXIQ_CR_IQSWAP              (0x1 << 3)
#define AXIQ_CR_CLEAR_ERROR         (0x1 << 4)

#define AXIQ_CR_THRESH_16           (0x0 << 1)
#define AXIQ_CR_THRESH_8            (0x1 << 1)
#define AXIQ_CR_THRESH_4            (0x2 << 1)
#define AXIQ_CR_THRESH_2            (0x3 << 1)

#define AXIQ_CR_SHIFT_FIFO_RX1      0
#define AXIQ_CR_SHIFT_FIFO_RX2      8
#define AXIQ_CR_SHIFT_FIFO_RX3      16
#define AXIQ_CR_SHIFT_FIFO_RX4      24
#define AXIQ_CR_SHIFT_FIFO_RX5      0
#define AXIQ_CR_SHIFT_FIFO_RX6      8

#define AXIQ_CR_SHIFT_FIFO_TX1      0
#define AXIQ_CR_SHIFT_FIFO_TX2      8
#define AXIQ_CR_SHIFT_FIFO_TX3      16
#define AXIQ_CR_SHIFT_FIFO_TX4      24
#define AXIQ_CR_SHIFT_FIFO_TX5      0

#define AXIQ_CR_FIELD_FIFO_RX1      (0x1F << AXIQ_CR_SHIFT_FIFO_RX1)
#define AXIQ_CR_FIELD_FIFO_RX2      (0x1F << AXIQ_CR_SHIFT_FIFO_RX2)
#define AXIQ_CR_FIELD_FIFO_RX3      (0x1F << AXIQ_CR_SHIFT_FIFO_RX3)
#define AXIQ_CR_FIELD_FIFO_RX4      (0x1F << AXIQ_CR_SHIFT_FIFO_RX4)
#define AXIQ_CR_FIELD_FIFO_RX5      (0x1F << AXIQ_CR_SHIFT_FIFO_RX5)
#define AXIQ_CR_FIELD_FIFO_RX6      (0x1F << AXIQ_CR_SHIFT_FIFO_RX6)

#define AXIQ_CR_FIELD_FIFO_TX1      (0x1F << AXIQ_CR_SHIFT_FIFO_TX1)
#define AXIQ_CR_FIELD_FIFO_TX2      (0x1F << AXIQ_CR_SHIFT_FIFO_TX2)
#define AXIQ_CR_FIELD_FIFO_TX3      (0x1F << AXIQ_CR_SHIFT_FIFO_TX3)
#define AXIQ_CR_FIELD_FIFO_TX4      (0x1F << AXIQ_CR_SHIFT_FIFO_TX4)
#define AXIQ_CR_FIELD_FIFO_TX5      (0x1F << AXIQ_CR_SHIFT_FIFO_TX5)

#define AXIQ_THRESH                 AXIQ_CR_THRESH_16

static inline void axiq_rx_enable(void)
{
    // Enable RX1, RX2, RX3:
    __ip_write(LA9310_GPO_AXIQ_CR0,
        AXIQ_CR_FIELD_FIFO_RX1|AXIQ_CR_FIELD_FIFO_RX2|AXIQ_CR_FIELD_FIFO_RX3,
        ((AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX1)|
        ((AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX2)|
        ((AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX3));
}

static inline void axiq_tx_enable(void)
{
    // Enable TX5:
    __ip_write(LA9310_GPO_AXIQ_CR3, AXIQ_CR_FIELD_FIFO_TX5,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX5);
}

static inline void axiq_enable_rx1(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX1,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX1);
}

static inline void axiq_enable_rx2(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX2,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX2);
}

static inline void axiq_enable_rx3(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX3,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX3);
}

static inline void axiq_enable_rx4(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX4,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX4);
}

static inline void axiq_enable_rx5(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR1, AXIQ_CR_FIELD_FIFO_RX5,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX5);
}

static inline void axiq_enable_rx6(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR1, AXIQ_CR_FIELD_FIFO_RX6,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_RX6);
}

static inline void axiq_enable_tx1(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX1,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX1);
}

static inline void axiq_enable_tx2(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX2,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX2);
}

static inline void axiq_enable_tx3(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX3,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX3);
}

static inline void axiq_enable_tx4(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX4,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX4);
}

static inline void axiq_enable_tx5(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR3, AXIQ_CR_FIELD_FIFO_TX5,
        (AXIQ_CR_ENABLE | AXIQ_THRESH) << AXIQ_CR_SHIFT_FIFO_TX5);
}

static inline void axiq_disable_rx1(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX1, 0);
}

static inline void axiq_disable_rx2(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX2, 0);
}

static inline void axiq_disable_rx3(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX3, 0);
}

static inline void axiq_disable_rx4(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR0, AXIQ_CR_FIELD_FIFO_RX4, 0);
}

static inline void axiq_disable_rx5(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR1, AXIQ_CR_FIELD_FIFO_RX5, 0);
}

static inline void axiq_disable_rx6(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR1, AXIQ_CR_FIELD_FIFO_RX6, 0);
}

static inline void axiq_disable_tx1(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX1, 0);
}

static inline void axiq_disable_tx2(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX2, 0);
}

static inline void axiq_disable_tx3(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX3, 0);
}

static inline void axiq_disable_tx4(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR2, AXIQ_CR_FIELD_FIFO_TX4, 0);
}

static inline void axiq_disable_tx5(void)
{
    __ip_write(LA9310_GPO_AXIQ_CR3, AXIQ_CR_FIELD_FIFO_TX5, 0);
}

/**
 * @brief       Configure the AXIQ for RXA, RXB, RXC, TXD.
 * @return      This function does not return a value.
 */
extern void axiq_configure(void);

/** @} */

/**
 * @defgroup    GROUP_AXIQ_STATUS   Status Interface
 * @{
 */
#define AXIQ_SR_ENABLE              (0x1 << 0)
#define AXIQ_SR_NOT_EMPTY           (0x1 << 1)
#define AXIQ_SR_NOT_FULL            (0x1 << 1)
#define AXIQ_SR_ERR_UNDERFLOW       (0x1 << 2)
#define AXIQ_SR_ERR_OVERFLOW        (0x1 << 3)

#define AXIQ_SR_SHIFT_FIFO_RX1      0
#define AXIQ_SR_SHIFT_FIFO_RX2      4
#define AXIQ_SR_SHIFT_FIFO_RX3      8
#define AXIQ_SR_SHIFT_FIFO_RX4      12
#define AXIQ_SR_SHIFT_FIFO_RX5      16
#define AXIQ_SR_SHIFT_FIFO_RX6      20

#define AXIQ_SR_SHIFT_FIFO_TX1      0
#define AXIQ_SR_SHIFT_FIFO_TX2      4
#define AXIQ_SR_SHIFT_FIFO_TX3      8
#define AXIQ_SR_SHIFT_FIFO_TX4      12
#define AXIQ_SR_SHIFT_FIFO_TX5      16

#define AXIQ_SR_FIELD_FIFO_RX1      (0xF << AXIQ_SR_SHIFT_FIFO_RX1)
#define AXIQ_SR_FIELD_FIFO_RX2      (0xF << AXIQ_SR_SHIFT_FIFO_RX2)
#define AXIQ_SR_FIELD_FIFO_RX3      (0xF << AXIQ_SR_SHIFT_FIFO_RX3)
#define AXIQ_SR_FIELD_FIFO_RX4      (0xF << AXIQ_SR_SHIFT_FIFO_RX4)
#define AXIQ_SR_FIELD_FIFO_RX5      (0xF << AXIQ_SR_SHIFT_FIFO_RX5)
#define AXIQ_SR_FIELD_FIFO_RX6      (0xF << AXIQ_SR_SHIFT_FIFO_RX6)

#define AXIQ_SR_FIELD_FIFO_TX1      (0xF << AXIQ_SR_SHIFT_FIFO_TX1)
#define AXIQ_SR_FIELD_FIFO_TX2      (0xF << AXIQ_SR_SHIFT_FIFO_TX2)
#define AXIQ_SR_FIELD_FIFO_TX3      (0xF << AXIQ_SR_SHIFT_FIFO_TX3)
#define AXIQ_SR_FIELD_FIFO_TX4      (0xF << AXIQ_SR_SHIFT_FIFO_TX4)
#define AXIQ_SR_FIELD_FIFO_TX5      (0xF << AXIQ_SR_SHIFT_FIFO_TX5)

/** @} */

/**
 * @defgroup    GROUP_AXIQ_DATA Data Interface
 * @{
 */

#define AXIQ_FIFO_RX1               (LA9310_BASE_AXIQ + 0x1000)    /**< AXIQ RX1 FIFO byte address. */
#define AXIQ_FIFO_RX2               (LA9310_BASE_AXIQ + 0x2000)    /**< AXIQ RX2 FIFO byte address. */
#define AXIQ_FIFO_RX3               (LA9310_BASE_AXIQ + 0x3000)    /**< AXIQ RX3 FIFO byte address. */
#define AXIQ_FIFO_RX4               (LA9310_BASE_AXIQ + 0x4000)    /**< AXIQ RX4 FIFO byte address. */
#define AXIQ_FIFO_RX5               (LA9310_BASE_AXIQ + 0x5000)    /**< AXIQ RX5 FIFO byte address. */
#define AXIQ_FIFO_RX6               (LA9310_BASE_AXIQ + 0x6000)    /**< AXIQ RX6 FIFO byte address. */

#define AXIQ_FIFO_TX1               (LA9310_BASE_AXIQ + 0x7000)    /**< AXIQ TX1 FIFO byte address. */
#define AXIQ_FIFO_TX2               (LA9310_BASE_AXIQ + 0x8000)    /**< AXIQ TX2 FIFO byte address. */
#define AXIQ_FIFO_TX3               (LA9310_BASE_AXIQ + 0x9000)    /**< AXIQ TX3 FIFO byte address. */
#define AXIQ_FIFO_TX4               (LA9310_BASE_AXIQ + 0xA000)    /**< AXIQ TX4 FIFO byte address. */
#define AXIQ_FIFO_TX5               (LA9310_BASE_AXIQ + 0xB000)    /**< AXIQ TX5 FIFO byte address. */


/**
 * @brief       AXIQ interface descriptor.
 *
 * This structure aggregates the parameters describing an AXIQ channel interface.
 *
 */
struct axiq_t {
    uint32_t addr;      /**< AXIQ FIFO byte address.                          */
    uint32_t chan;      /**< AXIQ FIFO DMA channel number.                    */
    uint32_t mask;      /**< AXIQ FIFO DMA channel mask.                      */
};

/**
 * @brief       DMA transfer control to write to an AXIQ interface.
 *
 * @note        Completion of a write transfer will assert an event on the VCPU.
 */
#define AXIQ_CTRL_WR    (DMA_WRC | DMA_FIFO | DMA_TRIG_EXT | DMA_GOEN_VCPU)

/**
 * @brief       DMA transfer control to read from an AXIQ interface.
 *
 * @note        Completion of a read transfer will assert an event on the VCPU.
 */
#define AXIQ_CTRL_RD    (DMA_RDC | DMA_FIFO | DMA_TRIG_EXT | DMA_GOEN_VCPU)

/**
 * @brief       Open an AXIQ stream.
 *
 * This function initializes an AXIQ interface descriptor with its FIFO address
 * and DMA channel.
 *
 * @param       this    the AXIQ interface descriptor address.
 * @param       addr    the AXIQ FIFO byte address.
 * @param       chan    the DMA channel number.
 * @return      This function does not return a value.
 */
extern void axiq_open(struct axiq_t *this, uint32_t addr, uint32_t chan);

/**
 * @brief       Close an AXIQ stream.
 *
 * This function aborts any current or pending DMA transfer on the AXIQ
 * interface descriptor specified by @a this.
 *
 * @param       this    the AXIQ interface descriptor address.
 * @return      This function does not return a value.
 */
extern void axiq_close(struct axiq_t *this);

/**
 * @brief       Read data from an AXIQ interface.
 *
 * This function reads @a size bytes from the AXIQ interface @a this into
 * the data memory buffer mapped at address @a addr.
 *
 * @param       this    the AXIQ interface descriptor address.
 * @param       addr    the data memory buffer byte address.
 * @param       size    the number of bytes to read.
 * @return      This function does not return a value.
 */


/**
 * @brief       Write data to an AXIQ interface.
 *
 * This function writes @a size bytes from the data memory buffer mapped at
 * address @a addr into the AXIQ interface @a this.
 *
 * @param       this    the AXIQ interface descriptor address.
 * @param       addr    the data memory buffer byte address.
 * @param       size    the number of bytes to write.
 * @return      This function does not return a value.
 */


/**
 * @brief       Number of samples received per AXIQ channel.
 */
#define AXIQ_SIZE_RX_DMA    128

/**
 * @brief       Number of receive buffers per AXIQ channel queue.
 */
#define AXIQ_SIZE_RX_QUEUE  2

/**
 * @brief       Receive AXIQ double-buffers.
 */


/**
 * @brief       Receive AXIQ control structure type.
 */
struct axiq_rx_ctrl_t {
    uint32_t index;     /**< Index in receive buffer queue of last received samples. */
};

/**
 * @brief       Receive AXIQ control structure.
 */
extern struct axiq_rx_ctrl_t axiq_rx_ctrl;

/**
 * @brief       Initialize the AXIQ receive interface.
 *
 * This function queues 2 DMA read transfers per receive AXIQ channel, to fill
 * the first 2 buffers in the AXIQ buffer queue.
 *
 * @return      This function does not return a value.
 */
extern void axiq_rx_init(void);

/**
 * @brief       Service the AXIQ receive event.
 *
 * This function programs 1 DMA read transfer per receive AXIQ channel, to fill
 * the next available buffer in the AXIQ buffer queue.
 *
 * @return      The index of the last received buffer in the buffer queue.
 */
extern uint32_t axiq_rx_service(void);





/** @} */
/** @} */

#endif /* __AXIQ_H__ */
