
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "system_resources.h"
#include "nr_pss.h"
#include "ippu.h"
#include "diffft.h"
#include "definitions.h"
#include "dma.h"

uint8_t cf_ret = 0x1;
__attribute__(( section(".text.ssb")))
uint32_t cell_follow_mib(void)
{   
	static uint8_t  pss_buffer_init  = 0x0;
	static uint32_t is_sss_det       = 0x0;
	static uint32_t is_pss_det       = 0x0;
	static uint32_t track_sss        = 0x0;
	static cfixed16_t* px_sss_input  = NULL;
	static cfixed16_t* px_pbch_input = NULL ;
	static uint8_t pbch_idx 		 = 0x0;
	
	cfloat16_t* px_fft_out        =  (cfloat16_t*)BUF_SCRATCH ;
	cfloat16_t* px_sssext_out     =  (cfloat16_t*)BUF_SCRATCH + 1024;
	fixed16_t*  sss_seq_fd    	  =  (fixed16_t* )REF_SEQ_BUF ;
	uint16_t  pbch_sym_offset[3]  =  {476, 440, 404};
	
	
	if(process_iteration == 1)
	{
    	is_pss_det 		 = 0;
    	pss_buffer_init  = 1;
		ssdet_out.id2    = (ssdet_out.cellid % 3);
		ssdet_out.id1    = (ssdet_out.cellid - ssdet_out.id2)/3;
		pbch_idx 		 = 0 ;
	}
  
	if (!is_pss_det) 
	{
		track_sss  = 0;
		is_pss_det = nr_pss_cell_follow_mib( pDcmOut_s2, CBUF_PROC, (uint32_t*)(BUF_SCRATCH), CBUF_SZ * 2 , pss_buffer_init);
		 	 	 	 
		if(is_pss_det)
		{
			px_sss_input  =  ssdet_out.ptrPssData_1536;
			ssb_info.t_delta_minor = (((px_sss_input - pDcmOut_s2 + (B_SZ << 2)) >> 2) << 2) + ((process_iteration - 1) << 10); //Samples@61.44Msps
		}
	}   
	
	if(track_sss==2)
	{
		sss_gen_hp_v1(BUF_SCRATCH, sss_seq_fd, ssdet_out.id1, ssdet_out.id2);
	}
	
	if((track_sss == 4) || (track_sss == 6) || (track_sss == 8))
	{
		px_pbch_input = (px_sss_input >= (CBUF_PROC + pbch_sym_offset[pbch_idx])) ? (px_sss_input - pbch_sym_offset[pbch_idx]) : (px_sss_input - pbch_sym_offset[pbch_idx] + CBUF_SZ);

		fftDIF512_hfx_hfx(px_pbch_input, (cfixed16_t*)px_fft_out, CBUF_PROC, CBUF_SZ<<1);	
		
		DMA_WR_COMP((uint32_t)(p_SSB_PBCH_SYM + (FFT_SIZE * pbch_idx * 2)), 2*((uint32_t)px_fft_out), FFT_SIZE*4);			
		dma_ch_busy_wait(DMA_DDR_WR_CH);

		pbch_idx = pbch_idx + 1;	
		
		if(pbch_idx == 3)
		{
			pbch_idx = 0;
			cf_ret = PBCH_CAPTURED;
			return cf_ret;
		}
	}
	
	if(is_pss_det == 1)
	{				
		if(track_sss == 6)
		{
			
			px_sss_input = (px_sss_input >= (CBUF_PROC + 440)) ? (px_sss_input - 440) : (px_sss_input - 440 + CBUF_SZ);
	
			fftDIF512_hfx_hfl(px_sss_input, (cfloat16_t* )BUF_SCRATCH, (cfixed16_t*)BUF_SCRATCH, SCBUF_SZ<<1);
						
			__ip_write( IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) fftBR512HP | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}

			__ip_write( IPREG_IPPU_CTRL, 0xFFFFFFFF, (unsigned int) extract_sss | (1 << 31) | (1 << 24));//turn on ippu and 16-bit dmem mode
			while ( __ip_read( IPREG_IPPU_STAT, 0x80000000 ) ) {}
			
			is_sss_det = sssdet_targeted_hpgemv_v1(px_sssext_out, BUF_SCRATCH, sss_seq_fd, ssdet_out.id1, ssdet_out.id2);
			
			px_sss_input = (px_sss_input >= CBUF_PROC +(CBUF_SZ - (440))) ? px_sss_input + (440) - CBUF_SZ : px_sss_input + (440);
		}
		  
		px_sss_input = (px_sss_input >= CBUF_PROC +(CBUF_SZ - (256))) ? px_sss_input + (256) - CBUF_SZ : px_sss_input + (256);
	   		
		track_sss++;
		
	    if(track_sss > 8)
	    {
		    is_pss_det = 0;
		    return SSB_NOT_DETECTED; 
	    }
	}
	return SSB_NOT_DETECTED; 
}
