
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "system_resources.h"
#include "nr_pss.h"
#include "ippu.h"
#include "diffft.h"
#include "definitions.h"
#include "dma.h"

 
__attribute__(( section(".text.cf")))
uint32_t cell_follow(void)
{   
	static uint8_t  pss_buffer_init  = 0x0;
	static uint32_t is_sss_det       = 0x0;
	static uint32_t is_pss_det       = 0x0;
	static uint32_t track_sss        = 0x0;
	static cfixed16_t* px_sss_input  = NULL;
	
	cfixed16_t* px_ref_pss_td  =  (cfixed16_t*)REF_SEQ_BUF ;
	cfixed16_t* px_align_pss   =  (cfixed16_t*)REF_SEQ_BUF + 512;
	cfixed16_t* px_deref_pss   =  (cfixed16_t*)REF_SEQ_BUF + 1024;//should be preserved till ffo estimation
	
	cfloat16_t* px_fft_out     =  (cfloat16_t*)BUF_SCRATCH ;
	cfloat16_t* px_bitrev_out  =  (cfloat16_t*)BUF_SCRATCH + 512;
	
	cfloat16_t* px_sssext_out  =  (cfloat16_t*)BUF_SCRATCH + 1024;
	cfixed16_t* px_sss_seq_fft =  (cfixed16_t*)BUF_SCRATCH + 1024;
	cfloat16_t* px_sss_deref   =  (cfloat16_t*)BUF_SCRATCH + 1024;
	
	cfixed16_t* px_sss_seq_td  =  (cfixed16_t*)BUF_SCRATCH + 1536;  //should be preserved till ffo estimation
	fixed16_t*  sss_seq_fd     =  (fixed16_t* )REF_SEQ_BUF ;
	
	cfixed16_t* sub_end_low    =  (cfixed16_t*)px_bitrev_out+64;
	cfixed16_t* buf_dec            		 = (cfixed16_t*)BUF_SCRATCH;
	cfixed16_t* re_noise_upp_half   = (cfixed16_t*)BUF_SCRATCH + PSS_NOISE_BUFF_OFFSET + PSS_NOISE_RE_HIGH_IDX;
	cfixed16_t* re_noise_low_half   = (cfixed16_t*)BUF_SCRATCH + PSS_NOISE_BUFF_OFFSET + PSS_NOISE_RE_LOW_IDX;
	
	if(process_iteration == 1)
	{
    	is_pss_det 		 = 0;
    	pss_buffer_init  = 1;
		ssdet_out.id2    = (ssdet_out.cellid % 3);
		ssdet_out.id1    = (ssdet_out.cellid - ssdet_out.id2)/3;
	}
  
	if (!is_pss_det) 
	{
		track_sss  = 0;
		is_pss_det = nr_pss_cell_follow( pDcmOut_s2, CBUF_PROC, (uint32_t*)(BUF_SCRATCH), CBUF_SZ * 2 , pss_buffer_init);
		 
		if(is_pss_det)
		{	
			px_sss_input  =  ssdet_out.ptrPssData_1536;		
			ssb_info.t_delta_minor = (((px_sss_input - pDcmOut_s2 + (B_SZ << 2)) >> 2) << 2) + ((process_iteration - 1) << 10); //Samples@61.44Msps
					
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
	
	if(track_sss==1)
	{
		//Dereference PSS 
		DMA_RD((uint32_t)(p_PSSDET_REF_PSSSEQTIME + ssdet_out.id2*FFT_SIZE*2) , 2*((uint32_t)REF_SEQ_BUF), FFT_SIZE*4);
		dma_ch_busy_wait(DMA_DDR_RD_CH);		
		vmult_conj_hfx_hfx(px_ref_pss_td,ssdet_out.ptrPssData_1536,px_deref_pss,CBUF_PROC,CBUF_SZ<<1,FFT_SIZE>>6); 

	}
	
	if(track_sss==2)
	{
		sss_gen_hp_v1(BUF_SCRATCH, sss_seq_fd, ssdet_out.id1, ssdet_out.id2);
		clr_buff_asm( (void*)px_sss_seq_fft, FFT_SIZE/SIZEVEC32);
		ifftshift_buff_format(sss_seq_fd,px_sss_seq_fft,px_sss_seq_fft+FFT_SIZE-64 );

		ifftDIF512_hfx_hfl(px_sss_seq_fft,px_fft_out, px_sss_seq_fft, FFT_SIZE<<1);
		
		__ip_write(IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
		while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
		
		veccpy( px_sss_seq_td,(cfixed16_t*)px_bitrev_out, FFT_SIZE/SIZEVEC32 );
	}
	
	if(is_pss_det == 1)
	{				
		if(track_sss > 5)
		{
			
			px_sss_input = (px_sss_input >= (CBUF_PROC + 440)) ? (px_sss_input - 440) : (px_sss_input - 440 + CBUF_SZ);

			float corr_re = 0;
			float corr_im = 0;
			
			fftDIF512_hfx_hfl(px_sss_input, px_fft_out, CBUF_PROC, CBUF_SZ<<1);

			__ip_write(IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}

			__ip_write(IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) extract_sss | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
			
			is_sss_det = sssdet_targeted_hpgemv_v1(px_sssext_out, BUF_SCRATCH, sss_seq_fd, ssdet_out.id1, ssdet_out.id2);
						
			// FFO Estimation steps 
			fftDIF512_hfx_hfx(px_sss_input,(cfixed16_t*)px_fft_out, CBUF_PROC, CBUF_SZ<<1);                    
			__ip_write(IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
			
			//Nulling PBCH in SSS
			clr_buff_asm( sub_end_low, (FFT_SIZE-128)/32);
			
			px_bitrev_out[63].real=0;
			px_bitrev_out[63].imag=0;
			
			//IFFT of SSS data 
			ifftDIF512_hfx_hfl((cfixed16_t*)px_bitrev_out,(cfloat16_t*)px_fft_out, (cfixed16_t*)px_bitrev_out, FFT_SIZE<<1);
			__ip_write(IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
			
			//Dereference SSS 
			vmult_conj_vecalgn_hfl_hfl((cfloat16_t*)px_sss_seq_td,(cfloat16_t*)px_bitrev_out,px_sss_deref,FFT_SIZE/SIZEVEC32);

			vmult_correlation_vecalgn_asm(px_deref_pss, px_sss_deref, &corr_re, &corr_im,512/128);
		
			if(is_sss_det == 1)
			{	
			
				is_pss_det = 0;
				ssdet_out.track_ffo_corr_re = corr_re;
				ssdet_out.track_ffo_corr_im = corr_im;
				return SSB_DETECTED;    
			}			
		}
		  
		px_sss_input = (px_sss_input >= CBUF_PROC +(CBUF_SZ - (256))) ? px_sss_input + (256) - CBUF_SZ : px_sss_input + (256);
	   		
		track_sss++;
		
	    if(track_sss > 6)
	    {
		    is_pss_det = 0;
		    return SSB_NOT_DETECTED; 
	    }
	}
	return SSB_NOT_DETECTED; 
}
