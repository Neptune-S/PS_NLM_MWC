#include <stdio.h>
#include <string.h>
#include "system_resources.h"
#include "nr_pss.h"
#include "ippu.h"
#include "diffft.h"
#include "definitions.h"
#include "dma.h"

__attribute__(( section(".text.ssb")))
uint32_t cell_search(uint32_t iFH)
{
    static cfixed16_t* px_sss_input;
    static uint32_t is_pss_det      = 0;
    static uint32_t is_sss_det      = 0;
	static uint32_t track_sss       = 0;
	static float max_est            = 0;
	float corr_est                  = 0;
	int32_t cfo_hyp                 = 0;
    cfixed16_t* buf_dec             = (cfixed16_t*)BUF_SCRATCH;
	cfixed16_t* pss_i_cfo_corr      = (cfixed16_t*)BUF_SCRATCH + 512;
	cfixed16_t* pss_deref           = (cfixed16_t*)BUF_SCRATCH + 1024;
    cfixed16_t* pss_f_cfo_corr      = (cfixed16_t*)BUF_SCRATCH + 1536;
	cfixed16_t* re_noise_upp_half   = (cfixed16_t*)BUF_SCRATCH + PSS_NOISE_BUFF_OFFSET + PSS_NOISE_RE_HIGH_IDX;
	cfixed16_t* re_noise_low_half   = (cfixed16_t*)BUF_SCRATCH + PSS_NOISE_BUFF_OFFSET + PSS_NOISE_RE_LOW_IDX;

	if(process_iteration == 1)
	{
      is_pss_det           = 0;
      ssdet_out.id1        = 0;
      ssdet_out.nco_psscfo = 0;
	}
	
	if (!is_pss_det) 
	{
		track_sss  = 0;
		is_pss_det = nr_pss_cell_search(pDcmOut_s2, CBUF_PROC, (uint32_t*)(buf_dec), CBUF_SZ * 2 * UPHW, iFH);
			   		
		if(is_pss_det)
		{
			px_sss_input  =  ssdet_out.ptrPssData_1536;
			
			fftDIF512_hfx_hfl(px_sss_input, (cfloat16_t* )buf_dec, (cfixed16_t*)CBUF_PROC, CBUF_SZ<<1);
			
			__ip_write( IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
						
			for(int i=0;i<8;i++)
			{
				buf_dec[PSS_NOISE_BUFF_OFFSET+PSS_NOISE_RE_HIGH_IDX+56+i].imag = 0;
				buf_dec[PSS_NOISE_BUFF_OFFSET+PSS_NOISE_RE_HIGH_IDX+56+i].real = 0;
				buf_dec[PSS_NOISE_BUFF_OFFSET+PSS_NOISE_RE_LOW_IDX+56+i].imag = 0;
				buf_dec[PSS_NOISE_BUFF_OFFSET+PSS_NOISE_RE_LOW_IDX+56+i].real = 0;
			}
						
			noise_estimation_asm(re_noise_upp_half, re_noise_low_half, &(ssdet_out.noise_est));

		}
	}                       
	
	if(track_sss == 1)
	{   
		int32_t ffo_low[5] = {-3355444,-1677722, 0, 1677722, 3355444};
		
		compensate_cfo_cplx(pss_i_cfo_corr, ssdet_out.ptrPssData_1536, (cfixed16_t*)CBUF_PROC, CBUF_SZ<<1,ssdet_out.nco_freqhypo,0,4);
		
		DMA_RD((uint32_t)(p_PSSDET_REF_PSSSEQTIME + ssdet_out.id2*FFT_SIZE*2) , 2*((uint32_t)REF_SEQ_BUF), FFT_SIZE*4);			
	    dma_ch_busy_wait(DMA_DDR_RD_CH);
	    
	    max_est = 0;
	
		for(uint8_t k = 0; k<5; k++)
		{  
			cfo_hyp   = ffo_low[k];
			compensate_cfo_cplx(pss_f_cfo_corr, pss_i_cfo_corr, (cfixed16_t*)BUF_SCRATCH, SCBUF_SZ<<1,cfo_hyp,0,4);
			//vmult_conj_vecalgn_hfx_hfx(REF_SEQ_BUF,pss_f_cfo_corr,pss_deref,FFT_SIZE/32);
			vmult_conj_hfx_hfx(REF_SEQ_BUF,pss_f_cfo_corr,pss_deref,(cfixed16_t*)BUF_SCRATCH,SCBUF_SZ<<1,FFT_SIZE>>6);
			corr_est = sum_512_hfx_power_asm(pss_deref,(cfixed16_t*)BUF_SCRATCH,SCBUF_SZ<<1);
						
		    if(corr_est >= max_est)
			{   
		    	ssdet_out.nco_psscfo = ssdet_out.nco_freqhypo + cfo_hyp;
				max_est  = corr_est;
			}
		}
		
	}
	
	if(track_sss == 2)
	{   
		
		int32_t ffo_high[4] = {-6710888,-5033166, 5033166, 6710888};
			
		for(uint8_t k = 0; k<4; k++)
		{  
		   cfo_hyp   = ffo_high[k];
		   compensate_cfo_cplx(pss_f_cfo_corr, pss_i_cfo_corr, (cfixed16_t*)BUF_SCRATCH, SCBUF_SZ<<1,cfo_hyp,0,4);
		   //vmult_conj_vecalgn_hfx_hfx(REF_SEQ_BUF,pss_f_cfo_corr,pss_deref,FFT_SIZE/32);
		   vmult_conj_hfx_hfx(REF_SEQ_BUF,pss_f_cfo_corr,pss_deref,(cfixed16_t*)BUF_SCRATCH,SCBUF_SZ<<1,FFT_SIZE>>6);
		   corr_est = sum_512_hfx_power_asm(pss_deref,(cfixed16_t *)BUF_SCRATCH,SCBUF_SZ<<1);
							
		   if(corr_est >= max_est)
		   {
			   ssdet_out.nco_psscfo = ssdet_out.nco_freqhypo + cfo_hyp;			   		    	
			   max_est  = corr_est;
		   }
		}	
	}
	
	if(is_pss_det == 1)
	{	
		if(track_sss > 5)
		{
		    float max_ffo = 0;
			float temp    = 0;
			cfloat16_t* px_fft_out    =  (cfloat16_t*)BUF_SCRATCH;
			cfloat16_t* px_bitrev_out =  (cfloat16_t*)BUF_SCRATCH + 512;
		    cfloat16_t* px_sssext_out =  (cfloat16_t*)BUF_SCRATCH + 1024;
			cfixed16_t* px_coarse_out =  (cfixed16_t*)BUF_SCRATCH + 1536;
					
			px_sss_input = (px_sss_input >= (CBUF_PROC + 440)) ? (px_sss_input - 440) : (px_sss_input - 440 + CBUF_SZ);
			
			compensate_cfo_cplx(px_coarse_out, px_sss_input, CBUF_PROC, CBUF_SZ<<1, ssdet_out.nco_psscfo , 0, 4);
			
			fftDIF512_hfx_hfl(px_coarse_out, (cfloat16_t* )BUF_SCRATCH, (cfixed16_t*)BUF_SCRATCH, SCBUF_SZ<<1);
						
			__ip_write( IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}

			__ip_write( IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) extract_sss | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}

			is_sss_det=sssdet_hpgemv_v1(px_sssext_out, BUF_SCRATCH+1536, ssdet_out.id2 );

			if(is_sss_det == 1)
			{						
				is_pss_det = 0;
				ssb_info.t0 += ((process_iteration  - 8) << 10);
				return SSB_DETECTED;
			}
		}
	    
		px_sss_input = (px_sss_input >= CBUF_PROC +(CBUF_SZ - (B_SZ))) ? px_sss_input + (B_SZ) - CBUF_SZ : px_sss_input + (B_SZ);
	   		
		track_sss++;
		
	    if(track_sss > 6)
	    {
		    is_pss_det = 0;
		    return SSB_NOT_DETECTED; 
	    }	
	}
	return SSB_NOT_DETECTED;
}
