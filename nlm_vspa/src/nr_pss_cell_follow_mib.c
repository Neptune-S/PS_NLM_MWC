// =============================================================================
//! @file           lte_pssdet_cell_follow.c
//! @brief          PSS detector for cell following implementation.
//! @copyright      Copyright 2021-2022 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */

#include <vspa/intrinsics.h>
#include "ditfft.h"
#include "diffft.h"
#include "nr_pss.h"
#include "vspa.h"
#include "dma.h"
#include "ippu.h"
#include "system_resources.h"
#include "vspa.h"

__attribute__(( section(".text.ssb")))
uint32_t nr_pss_cell_follow_mib(
        cfixed16_t *px, 
        cfixed16_t *px_cbuf_beg,
        uint32_t   *pscr_buf,
        size_t     cbuf_size, 
        uint32_t   isInitBuffer) 
{   

    cfixed16_t* pfft_in;     
    cfixed16_t *pXFhf               = (cfixed16_t*)(pscr_buf);
    cfixed16_t *pYFhf               = (cfixed16_t*)(pscr_buf +  512);
    cfixed16_t *pYThf               = pYFhf; 
    float *pwr_x                    = (float*)(pscr_buf + 1024); 
    float *pwr_x_rcp                = (float*)(pscr_buf + 1024 + 256); 
    float *pxcn                     = (float*)pYFhf;
    float       power_corr;
	float       peak_val;
	float       max_peak = 0x0;
	uint32_t    peak_idx;
    uint32_t    is_pss_det = 0x0;
    
    pfft_in = (px < px_cbuf_beg + B_SZ) ? (px - B_SZ + CBUF_SZ) : (px - B_SZ);
    
    size_t nLineOut_cHF  = FFT_SIZE / SIZEVEC32;
    size_t nLineOut_rSP2 = (FFT_SIZE>>1) / SIZEVEC32;

    // Find FFT input starting point 
    fftDIF512_hfx_hfx( pfft_in, pXFhf, px_cbuf_beg, cbuf_size);
    
    cfixed16_t *pHhf = (cfixed16_t*)REF_SEQ_BUF;     

    DMA_RD((uint32_t)(p_PSSDET_REF_XCORRFFT) , 2*((uint32_t)REF_SEQ_BUF), PSS_NUM_ID2*FFT_SIZE*4);			
	dma_ch_busy_wait(DMA_DDR_RD_CH);
    
    pHhf += FFT_SIZE*ssdet_out.id2;

	vmult_vecalgn( pYFhf, pXFhf, pHhf, nLineOut_cHF );
	ifftDIT512_hfx_hfx( pYFhf, pYThf );
	xcn_hf_asm_modified(pxcn, pYThf, &peak_val,&peak_idx);
	power_corr = power_sum_256_sp_asm(pxcn,px_cbuf_beg,cbuf_size);

    if(peak_val > (pss_init_params.pssThresh*power_corr + 0.000001))  
    {
        is_pss_det = 1;
		ssdet_out.ptrPssData_1536 = pfft_in + peak_idx;
    }	
    
    return is_pss_det;
}
