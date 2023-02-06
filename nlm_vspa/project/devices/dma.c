// =============================================================================
//! @file           dma.c
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

#include "definitions.h"
#include "dma.h"


void dma_generic(uint32_t ctrl, uint32_t axi, uint32_t vsp, size_t size)
{
    __ip_write(DMA_DMEM_ADDR >> 2, vsp);
    __ip_write(DMA_AXI_ADDR >> 2,  axi);
    __ip_write(DMA_BYTE_CNT >> 2,  size);
    __ip_write(DMA_XFER_CTRL >> 2, ctrl);
}

uint32_t ext_go_stat_reg_read()
{
	return __iprd(EXT_GO_STAT);
}

void dma_ch_init(uint32_t ch)
{
	DMA_ABORT_CHANNEL(ch);
	DMA_CLEAR_STATUS(ch);
}

void dma_ch_busy_wait(uint32_t ch)
{
	uint32_t dma_comp;

	do { dma_comp = DMA_READ_STATUS(ch); } while (dma_comp == 0);
	DMA_CLEAR_STATUS(ch);
}

void dma_while_xfer(uint32_t ch)
{
	uint32_t dma_comp;

	do { dma_comp = DMA_READ_TRANSFER(ch); } while (dma_comp);
	DMA_CLEAR_STATUS(ch);
}

