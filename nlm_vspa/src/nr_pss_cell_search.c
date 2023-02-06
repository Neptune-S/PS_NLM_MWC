//--------------------------------------------------------------------------
// Copyright (c) 2019, NXP
// NXP SEMICONDUCTOR CONFIDENTIAL PROPRIETARY
//
//--------------------------------------------------------------------------

// =============================================================================
//! @file           pssdet_xcorr_fft_3072msps.c
//! @brief          PSS detector for 5GNR. Input at 30.72 MSPS.
//! @author         NXP Semiconductors.
//! @copyright      Copyright (C) 2017-2018 NXP Semiconductors.
// =============================================================================

//#pragma optimization_level 4

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <vspa/intrinsics.h>

//#include "vspa.h"
//#include "iohw.h"
//#include "vcpu.h"
//#include "ippu.h"
//
#include "ditfft.h"
#include "diffft.h"
#include "nr_pss.h"
#include "vspa.h"
#include "dma.h"
#include "system_resources.h"

__attribute__(( section(".text.ssb")))
uint32_t nr_pss_cell_search(
		cfixed16_t *px, 
		cfixed16_t *px_cbuf_beg,
		uint32_t *pscr_buf,
		size_t cbuf_size,
		uint32_t iFH) 
{
	
    cfixed16_t* pfft_in;
    cfixed16_t*	pfft_in_cfo_derot   = (cfixed16_t*)(pscr_buf);
    cfixed16_t *pYFhf    			= (cfixed16_t*)(pscr_buf );           
    cfixed16_t *pXFhf    		    = (cfixed16_t*)(pscr_buf + 512 ) ;
    cfixed16_t *pYThf               =  pYFhf; 
    float *pwr_x                	= (float*)(pscr_buf + 1024 );
    float *pwr_x_rcp                = (float*)(pscr_buf + 1024 + 256);
    float *pxcn                     = (float*)pYFhf;
    float power_corr;
	float peak_val;
	float max_peak            = 0;
    uint32_t    peak_idx;
    uint32_t    is_pss_det    = 0x0;
    uint32_t    id2           = 0x1;

	pfft_in = (px < px_cbuf_beg + B_SZ) ? (px - B_SZ + (CBUF_SZ)) : (px - B_SZ); 
	    
	compensate_cfo_cplx(pfft_in_cfo_derot, pfft_in, (cfixed16_t*)CBUF_PROC, CBUF_SZ<<1, pssdet_param.nco5x1[iFH],0,4);
	    
	size_t nLineOut_cHF  =  FFT_SIZE / SIZEVEC32;
	size_t nLineOut_rSP2 = (FFT_SIZE>>1) / SIZEVEC32;
	    	    
	fftDIF512_hfx_hfx( pfft_in_cfo_derot, pXFhf, (cfixed16_t*)pscr_buf, SCBUF_SZ);
    
    cfixed16_t *pHhf = (cfixed16_t*)REF_SEQ_BUF;

	DMA_RD((uint32_t)(p_PSSDET_REF_XCORRFFT) , 2*((uint32_t)REF_SEQ_BUF), PSS_NUM_ID2*FFT_SIZE*4);			
	dma_ch_busy_wait(DMA_DDR_RD_CH);
	
	for(id2 = 0; id2<PSS_NUM_ID2;++id2) 
	{  
		vmult_vecalgn( pYFhf, pXFhf, pHhf, nLineOut_cHF );
		ifftDIT512_hfx_hfx( pYFhf, pYThf );		
		xcn_hf_asm_modified(pxcn, pYThf, &peak_val,&peak_idx);
		power_corr = power_sum_256_sp_asm(pxcn,px_cbuf_beg,cbuf_size);
		
		if(peak_val > (pss_init_params.pssThresh*power_corr+0.000001))
		{	
			if(peak_val > max_peak)
			{	
				is_pss_det                = 1;
				ssdet_out.id2             = id2;
				ssdet_out.ptrPssData_1536 = pfft_in + peak_idx;
				max_peak                  = peak_val;
				ssdet_out.nco_freqhypo    = pssdet_param.nco5x1[iFH];
			}	
		}
		 pHhf += FFT_SIZE;
	}	

  return is_pss_det;
  
}
