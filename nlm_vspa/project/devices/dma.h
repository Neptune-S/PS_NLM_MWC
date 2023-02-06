// =============================================================================
//! @file           dma.h
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
#ifndef DMA_H_
#define DMA_H_

#include "system_resources.h"
#include "la9310.h"
#include "vspa.h"
#include "axiq.h"
#include "iohw.h"

#define DMA_DMEM_ADDR 		0xB0
#define DMA_AXI_ADDR 		0xB4
#define DMA_BYTE_CNT 		0xB8
#define DMA_XFER_CTRL 		0xBC

#define SCR_BUFF_HFIX_SIZE 			(1824)
#define SCR_BUFF_SP_SIZE 			(2848)
#define DDR_READ_SIZE			    (1024) << 2

#define DMA_STATUS_CTRL			0xC0
#define DMA_COMPLETE_STATUS		0xC8
#define DMA_CFGERR_STATUS		0xD0
#define DMA_GO_STATUS			0xD8
#define DMA_TRANSFER_ONGOING    0xD4

#define DMA_ABORT_CHANNEL(dma_channel)		__ipwr(DMA_STATUS_CTRL >> 2 , 0x1 << dma_channel , 0x1 << dma_channel)
#define DMA_CLEAR_STATUS(dma_channel)		__ipwr(DMA_COMPLETE_STATUS >> 2 , 0x1 << dma_channel , 0x1 << dma_channel)
#define DMA_READ_STATUS(dma_channel)		__iprd(DMA_COMPLETE_STATUS >> 2 , 0x1 << dma_channel)
#define DMA_READ_TRANSFER(dma_channel)		__iprd(DMA_TRANSFER_ONGOING >> 2 , 0x1 << dma_channel)
#define DMA_CLEAR_GO_STATUS(dma_channel)	__ipwr(DMA_GO_STATUS >> 2, 0x1 << dma_channel , 0x1 << dma_channel)
#define DMA_CLEAR_CFGERR(dma_channel)		__ipwr(DMA_CFGERR_STATUS >> 2, 0x1 << dma_channel , 0x1 << dma_channel)
#define DMA_READ_ERROR(dma_channel)			__iprd(DMA_CFGERR_STATUS >> 2 , 0x1 << dma_channel)


/* Transfer mode of DMA is set to 1, bits 8-10.
 * This transforms 2's complement to sign/magnitude
 * */
#define DMA_MODE(x) (x << 8)
#define TM_AXI_DMEM_NO_CONVERSION		0
#define TM_AXI_DMEM_2C_SM_CONVERSION	1
#define TM_AXI_VCPU_PRAM				2
#define TM_AXI_IPPU_PRAM				3
#define TM_DMEM_AXI						4
#define TM_RESERVED						5
#define TM_DMEM_AXI_NO_CONVERSION		6
#define TM_DMEM_AXI_SM_2C_CONVERSION	7

#define DMA_DI_8        ((0x1 << 18)|(0x4 << 8))    /**<  8-bit de-interleave mode.         */
#define DMA_DI_4LO      ((0x2 << 18)|(0x4 << 8))    /**<  4-bit de-interleave mode (LSBs).  */
#define DMA_DI_4HI      ((0x3 << 18)|(0x4 << 8))    /**<  4-bit de-interleave mode (MSBs).  */
#define DMA_DI_16       ((0x4 << 18)|(0x4 << 8))    /**< 16-bit de-interleave mode.         */
#define DMA_DI_32       ((0x5 << 18)|(0x4 << 8))    /**< 32-bit de-interleave mode.         */

#define EXT_TRIG		(1 << 16)
#define PTR_RESET		(1 << 15)
#define VCPU_GO_EN		(1 << 13)
#define BURST_FIXED		(1 << 11)
#define MULTI_BURST		(1 << 7)
#define COMP_VSPA_GO_BIT_0 (0)	//0 = COMP-0, 1 = COMP-12
#define COMP_VSPA_GO_BIT_1 (1)

#define ext_go_enable(bit, value)		 	__ipwr(EXT_GO_ENA,  (value | (1 << bit)))
#define ext_trig_go_clear(bit, value)		 __ipwr(EXT_GO_STAT, (value & (1 << bit)))


extern void dma_generic(uint32_t ctrl, uint32_t axi, uint32_t vsp, size_t size);
extern uint32_t ext_go_stat_reg_read();
extern void dma_ch_init(uint32_t ch);
extern void dma_ch_busy_wait(uint32_t ch);
extern void dma_while_xfer(uint32_t ch);

#define CMD_RX_PTR_RST 		            (PTR_RESET | BURST_FIXED | DMA_MODE(TM_AXI_DMEM_2C_SM_CONVERSION) | DMA_RX_RO2_CH)
#define CMD_TX_PTR_RST 		            (PTR_RESET | BURST_FIXED | DMA_MODE(TM_DMEM_AXI_2C_SM_CONVERSION) | DMA_TX_CH)
#define CMD_DDR_READ   		            (DMA_MODE(TM_AXI_DMEM_NO_CONVERSION) | DMA_DDR_RD_CH | MULTI_BURST)
#define CMD_DDR_READ_COMP 		        (DMA_MODE(TM_AXI_DMEM_2C_SM_CONVERSION) | DMA_DDR_RD_CH | MULTI_BURST)
#define CMD_DDR_WRITE  		            (VCPU_GO_EN | DMA_MODE(TM_DMEM_AXI_NO_CONVERSION) | DMA_DDR_WR_CH)
#define  CMD_DDR_WRITE_COMP             (VCPU_GO_EN | DMA_MODE(TM_DMEM_AXI_SM_2C_CONVERSION) | DMA_DDR_WR_CH)
#define CMD_PHYTMR_CTRL 	            (DMA_DI_32 | DMA_PHY_TIMER)
#define CMD_SYM_READ_AXIQ	            (EXT_TRIG | BURST_FIXED | VCPU_GO_EN | DMA_MODE(TM_AXI_DMEM_2C_SM_CONVERSION) | DMA_RX_RO2_CH)
#define CMD_SYM_READ_DDR   	            (VCPU_GO_EN | DMA_MODE(TM_AXI_DMEM_NO_CONVERSION) | DMA_RX_RO2_CH)
#define CMD_PRG_LOAD   		            (DMA_MODE(TM_AXI_VCPU_PRAM) | DMA_PROGRAM_CH)


#define RX_PTR_RESET 				    dma_generic(CMD_RX_PTR_RST, AXIQ_RX_RO2_ADDR, 2*(uint32_t)ippu_buffer, 4*DMA_TXR_size)
#define TX_PTR_RESET 				    dma_generic(CMD_TX_PTR_RST, AXIQ_TX_ADDR, 2*(uint32_t)transition_vector, 4*DMA_TXR_size)
#define DMA_RD(ddr, vspa, sz)		    dma_generic(CMD_DDR_READ, ddr, vspa, sz)
#define DMA_RD_COMP(ddr, vspa, sz)      dma_generic(CMD_DDR_READ_COMP, ddr, vspa, sz)
#define DMA_WR(ddr, vspa, sz)		    dma_generic(CMD_DDR_WRITE, ddr, vspa, sz)
#define DMA_WR_COMP(ddr, vspa, sz)	    dma_generic(CMD_DDR_WRITE_COMP, ddr, vspa, sz)
#define PHY_TMR_PRG(reg, cfg, sz)	    dma_generic(CMD_PHYTMR_CTRL, 2*reg, 2*cfg, sz)
#define SYM_HALF_GET(cmd, half, src)    dma_generic(cmd, src, 2*(uint32_t)&ippu_buffer[half*AXIQ_UNIT_TXFR_SIZE], AXIQ_UNIT_TXFR_BYTES)
#define PROG_LOAD(ovl_cfg)			    dma_generic(CMD_PRG_LOAD, ovl_cfg.src<<1, ovl_cfg.dst<<1, ovl_cfg.sz<<1)


#endif /* DMA_H_ */
