// =============================================================================
//! @file           la9310.h
//! @brief          Header containing register offsets.
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
#ifndef __LA9310_H__
#define __LA9310_H__

/**
 * @addtogroup  GROUP_LA9310
 * @{
 */

/**
 * @defgroup    GROUP_LA9310_AXI System Memory Map
 * @brief       Addresses and sizes of the various blocks on the AXI bus.
 *
 * This section lists the various blocks accessible by VSPA on the AXI bus.
 * All addresses and sizes are given in bytes.
 *
 * @{
 */

#define LA9310_BASE_DTCM           0x20000000              /**< Data TCM base address.         */
#define LA9310_BASE_CCSR           0x40000000              /**< CCSR base address.             */
#define LA9310_BASE_AXIQ           0x44000000              /**< AXIQ base address.             */
#define LA9310_BASE_LLCP           0x44010000              /**< LLCP base address.             */
#define LA9310_BASE_HOST           0xA0000000              /**< PCIe/Host memory base address. */

#define LA9310_SIZE_DTCM           (128 * 1024)            /**< Data TCM byte size.            */
#define LA9310_SIZE_HOST           (1024 * 1024 * 1024)    /**< Host memory byte size.         */

/** @} */

/**
 * @defgroup    GROUP_LA9310_CCSR  CCSR Address Map
 * @brief       Addresses of the various blocks in the CCSR address space.
 * @{
 */

#define LA9310_CCSR_BASE_VSPA          (LA9310_BASE_CCSR + 0x1000000)     /**< VSPA base address.                             */
#define LA9310_CCSR_BASE_PHY_TIMER     (LA9310_BASE_CCSR + 0x1020000)     /**< PHY Timer base address.                        */
#define LA9310_CCSR_BASE_ADC_DAC       (LA9310_BASE_CCSR + 0x1040000)     /**< ADC/DAC base address.                          */
#define LA9310_CCSR_BASE_AEM           (LA9310_BASE_CCSR + 0x1200000)     /**< Address Error Manager base address.            */
#define LA9310_CCSR_BASE_CGU           (LA9310_BASE_CCSR + 0x1360000)     /**< Platform Clock Generation Unit base address.   */
#define LA9310_CCSR_BASE_CLOCK         (LA9310_BASE_CCSR + 0x1370000)     /**< Clocking registers base address.               */
#define LA9310_CCSR_BASE_DCFG          (LA9310_BASE_CCSR + 0x1E00000)     /**< Device Configuration base address.             */
#define LA9310_CCSR_BASE_PMU           (LA9310_BASE_CCSR + 0x1E30000)     /**< Power Management Unit base address.            */
#define LA9310_CCSR_BASE_RESET         (LA9310_BASE_CCSR + 0x1E60000)     /**< Reset base address.                            */
#define LA9310_CCSR_BASE_SERDES        (LA9310_BASE_CCSR + 0x1EA0000)     /**< SerDes base address.                           */
#define LA9310_CCSR_BASE_TMU           (LA9310_BASE_CCSR + 0x1F80000)     /**< Thermal Monitoring Unit base address.          */
#define LA9310_CCSR_BASE_MESSAGE       (LA9310_BASE_CCSR + 0x1FC0000)     /**< Message Unit base address.                     */
#define LA9310_CCSR_BASE_PIN_CONTROL   (LA9310_BASE_CCSR + 0x1FF0000)     /**< Pin Control base address.                      */
#define LA9310_CCSR_BASE_I2C1          (LA9310_BASE_CCSR + 0x2000000)     /**< I2C #1 base address.                           */
#define LA9310_CCSR_BASE_I2C2          (LA9310_BASE_CCSR + 0x2010000)     /**< I2C #2 base address.                           */
#define LA9310_CCSR_BASE_SPI           (LA9310_BASE_CCSR + 0x2100000)     /**< SPI base address.                              */
#define LA9310_CCSR_BASE_UART          (LA9310_BASE_CCSR + 0x21C0000)     /**< UART base address.                             */
#define LA9310_CCSR_BASE_EDMA          (LA9310_BASE_CCSR + 0x22C0000)     /**< eDMA base address.                             */
#define LA9310_CCSR_BASE_LLCP          (LA9310_BASE_CCSR + 0x22E0000)     /**< LLCP base address.                             */
#define LA9310_CCSR_BASE_GPIO          (LA9310_BASE_CCSR + 0x2300000)     /**< GPIO base address.                             */
#define LA9310_CCSR_BASE_WDT           (LA9310_BASE_CCSR + 0x23C0000)     /**< Watchdog Timer base address.                   */
#define LA9310_CCSR_BASE_PCIE          (LA9310_BASE_CCSR + 0x3400000)     /**< PCIexpress Controller PF0 base address.        */
#define LA9310_CCSR_BASE_PCIE_CTRL     (LA9310_BASE_CCSR + 0x34C0000)     /**< PCI express Controller PFO controls.           */

/** @} */

/**
 * @defgroup    GROUP_LA9310_VSPA VSPA Memory Sizes
 * @{
 */

#ifdef __CAS__

#define LA9310_SIZE_VCPU_PRAM      (32*1024)   /**< VCPU program memory byte size. */
#define LA9310_SIZE_VCPU_DRAM      (32*1024)   /**< VCPU data memory byte size. */

#define LA9310_SIZE_IPPU_PRAM      (2*1024)    /**< IPPU program memory byte size. */
#define LA9310_SIZE_IPPU_DRAM      (4*1024)    /**< IPPU data memory byte size. */

#else // !__CAS__

#define LA9310_SIZE_VCPU_PRAM      (32*1024)   /**< VCPU program memory byte size. */
#define LA9310_SIZE_VCPU_DRAM      (32*1024)   /**< VCPU data memory byte size. */

#define LA9310_SIZE_IPPU_PRAM      (2*1024)    /**< IPPU program memory byte size. */
#define LA9310_SIZE_IPPU_DRAM      (16*1024)   /**< IPPU data memory byte size. */

#endif  // __CAS__

/** @} */

/**
 * @defgroup    GROUP_LA9310_DMA   DMA Channel Allocation
 * @{
 */

#define LA9310_DMA_CHAN_UNUSED_0   0       /**< Unused DMA channel.                                */
#define LA9310_DMA_CHAN_AXIQ_RX1   1       /**< DMA channel for AXIQ receive channel A.            */
#define LA9310_DMA_CHAN_AXIQ_RX2   2       /**< DMA channel for AXIQ receive channel B.            */
#define LA9310_DMA_CHAN_AXIQ_RX3   3       /**< DMA channel for AXIQ receive channel C.            */
#define LA9310_DMA_CHAN_AXIQ_RX4   4       /**< DMA channel for AXIQ receive channel D.            */
#define LA9310_DMA_CHAN_AXIQ_RX5   5       /**< DMA channel for AXIQ auxiliary receive channel.    */
#define LA9310_DMA_CHAN_AXIQ_RX6   6       /**< DMA channel for AXIQ RSSI channel.                 */
#define LA9310_DMA_CHAN_AXIQ_TX5   11      /**< DMA channel for AXIQ transmit channel.             */
#define LA9310_DMA_CHAN_HOST_RD    12      /**< DMA channel for data generation.                   */
#define LA9310_DMA_CHAN_GAIN_CMD   13      /**< DMA channel for gain update command.               */
#define LA9310_DMA_CHAN_PHY_TIMER  14      /**< DMA channel for PHY timer configuration/control.   */
#define LA9310_DMA_CHAN_HOST_WR    15      /**< DMA channel to upload decoded data from MFI task.  */

#define LA9310_DMA_MASK_UNUSED_0   (0x1 << LA9310_DMA_CHAN_UNUSED_0 )     /**< Unused DMA channel.                            */
#define LA9310_DMA_MASK_AXIQ_RX1   (0x1 << LA9310_DMA_CHAN_AXIQ_RX1 )     /**< Bit mask for AXIQ receive channel A.           */
#define LA9310_DMA_MASK_AXIQ_RX2   (0x1 << LA9310_DMA_CHAN_AXIQ_RX2 )     /**< Bit mask for AXIQ receive channel B.           */
#define LA9310_DMA_MASK_AXIQ_RX3   (0x1 << LA9310_DMA_CHAN_AXIQ_RX3 )     /**< Bit mask for AXIQ receive channel C.           */
#define LA9310_DMA_MASK_AXIQ_RX4   (0x1 << LA9310_DMA_CHAN_AXIQ_RX4 )     /**< Bit mask for AXIQ receive channel D.           */
#define LA9310_DMA_MASK_AXIQ_RX5   (0x1 << LA9310_DMA_CHAN_AXIQ_RX5 )     /**< Bit mask for AXIQ auxiliary receive channel.   */
#define LA9310_DMA_MASK_AXIQ_RX6   (0x1 << LA9310_DMA_CHAN_AXIQ_RX6 )     /**< Bit mask for AXIQ RSSI channel.                */
#define LA9310_DMA_MASK_AXIQ_TX5   (0x1 << LA9310_DMA_CHAN_AXIQ_TX5 )     /**< Bit mask for AXIQ transmit channel.            */
#define LA9310_DMA_MASK_HOST_RD    (0x1 << LA9310_DMA_CHAN_HOST_RD  )     /**< Bit mask for data generation.                  */
#define LA9310_DMA_MASK_GAIN_CMD   (0x1 << LA9310_DMA_CHAN_GAIN_CMD )     /**< Bit mask for gain update command.              */
#define LA9310_DMA_MASK_PHY_TIMER  (0x1 << LA9310_DMA_CHAN_PHY_TIMER)     /**< Bit mask for PHY timer configuration/control.  */
#define LA9310_DMA_MASK_HOST_WR    (0x1 << LA9310_DMA_CHAN_HOST_WR  )     /**< Bit mask for decoded data from MFI task.       */

/**
 * @brief       Bit mask for all active AXIQ receive channels.
 */
#ifndef __UC9__
#define LA9310_DMA_MASK_RX (   \
    LA9310_DMA_MASK_AXIQ_RX1 | \
    LA9310_DMA_MASK_AXIQ_RX2 | \
    LA9310_DMA_MASK_AXIQ_RX3 | \
    LA9310_DMA_MASK_AXIQ_RX4)
#else
#define LA9310_DMA_MASK_RX (   \
    LA9310_DMA_MASK_AXIQ_RX1 | \
    LA9310_DMA_MASK_AXIQ_RX2 | \
    LA9310_DMA_MASK_AXIQ_RX3)
#endif

/** @} */

/**
 * @defgroup    GROUP_LA9310_GPIO  GPIO Registers
 * @{
 */

#define GPO(x)  ((0x580 >> 2) + (x))

#define LA9310_GPO_AXIQ_CR0        GPO(4)      /**< VSPA GPO for AXIQ configuration. */
#define LA9310_GPO_AXIQ_CR1        GPO(5)      /**< VSPA GPO for AXIQ configuration. */
#define LA9310_GPO_AXIQ_CR2        GPO(6)      /**< VSPA GPO for AXIQ configuration. */
#define LA9310_GPO_AXIQ_CR3        GPO(7)      /**< VSPA GPO for AXIQ configuration. */

#define LA9310_GPI_AXIQ_SR0        GPI(0)      /**< VSPA GPI for AXIQ status. */
#define LA9310_GPI_AXIQ_SR1        GPI(1)      /**< VSPA GPI for AXIQ status. */
#define LA9310_GPI_AXIQ_SR2        GPI(2)      /**< VSPA GPI for AXIQ status. */
#define LA9310_GPI_AXIQ_SR3        GPI(3)      /**< VSPA GPI for AXIQ status. */

/** @} */

/**
 * @defgroup    GROUP_LA9310_AXIQ  AXIQ Hardware Interface
 * @{
 */

/**
 * @brief       Receive AXIQ channel enumeration.
 */
enum LA9310_chan_rx_e {
    LA9310_CHAN_RXA,       /**< Receive AXIQ channel A.                          */
    LA9310_CHAN_RXB,       /**< Receive AXIQ channel B.                          */
    LA9310_CHAN_RXC,       /**< Receive AXIQ channel C.                          */
    LA9310_CHAN_RXD,       /**< Receive AXIQ channel D.                          */
    LA9310_CHAN_RX_COUNT   /**< Total number of receive AXIQ channels.           */
};

/**
 * @brief       Transmit AXIQ channel enumeration.
 */
enum LA9310_chan_tx_e {
    LA9310_CHAN_TX,        /**< Transmit AXIQ channel.                           */
    LA9310_CHAN_TX_COUNT   /**< Total number of transmit AXIQ channels.          */
};

/** @} */

/** @} */

#endif /* __LA9310_H__ */
