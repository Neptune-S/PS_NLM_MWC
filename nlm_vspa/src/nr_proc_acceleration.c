#include "all_drivers.h"
#include "../include/nr_pss.h"
#include "../include/nr_proc_acceleration.h"
#include "system_resources.h"


__attribute__(( section(".text.cf")))
void pbch_rd_dtcm(cfixed16_t* px1, cfixed16_t* px2, uint16_t sz1,uint16_t sz2)
{
	DMA_RD_COMP((uint32_t)VSPA_DATA_IN_A_ADDR, 2*(uint32_t)px1,sz1 );
	dma_ch_busy_wait(DMA_DDR_RD_CH);
	DMA_RD_COMP((uint32_t)VSPA_DATA_IN_B_ADDR, 2*(uint32_t)px2,sz2);
	dma_ch_busy_wait(DMA_DDR_RD_CH);
	
}

__attribute__(( section(".text.cf")))
void pbch_dmrs_extraction()
{
	
		cfixed16_t* dmrs_seq  	= (cfixed16_t*)BUF_SCRATCH;  //144*8 
		cfixed16_t* dmrs_re     = (cfixed16_t*)BUF_SCRATCH + 1152; //144  
		//cfixed16_t* dmrs_corr   = (cfixed16_t*)BUF_SCRATCH + 1152 + (144+48); 
		float* corr_val   	    = (float*)BUF_SCRATCH + 1152 + (144+48)+(144+48);
	
		pbch_rd_dtcm( dmrs_seq, dmrs_re,REF_DMRS_SZ,RX_DMRS_SZ);
		ssb_idx_search(dmrs_seq,dmrs_re,corr_val);
		
		DMA_WR((uint32_t)VSPA_DATA_OUT_A_ADDR, 2*(uint32_t)corr_val, 8*4);
		dma_ch_busy_wait(DMA_DDR_WR_CH);
	
		procLSB   = 0;
		procMSB   = (procMSB  & 0xFF000000) |  (0x1) ;
		mbox1_send(procMSB, procLSB);	
}
__attribute__(( section(".text.cf")))
void pbch_ls_estimate()
{
		cfixed16_t* dmrs_seq  	= (cfixed16_t*)BUF_SCRATCH;  //144 + 48 = 192
		cfixed16_t* dmrs_re   	= (cfixed16_t*)BUF_SCRATCH + 192; //144
		cfixed16_t* ls_est      = (cfixed16_t*)BUF_SCRATCH + 192+192;
		
		pbch_rd_dtcm( dmrs_seq,  dmrs_re,RX_DMRS_SZ,RX_DMRS_SZ);
		vmult_conj_hfx_hfx(dmrs_seq,dmrs_re,ls_est,dmrs_re,DATA_RE_BUFF_SZ<<1, 192/64);
		DMA_WR_COMP((uint32_t)VSPA_DATA_OUT_A_ADDR, 2*(uint32_t)ls_est, RX_DMRS_SZ);
		dma_ch_busy_wait(DMA_DDR_WR_CH);
		
		procLSB   = 0 ;
		procMSB   = (procMSB  & 0xFF000000) |  (0x1) ;
		mbox1_send(procMSB, procLSB);
}

__attribute__(( section(".text.cf")))
void pbch_equalizer()
{
		
		cfixed16_t* channel_est  	= (cfixed16_t*)BUF_SCRATCH;  //144*3 + 16= 448
		cfixed16_t* data_re_in  	= (cfixed16_t*)BUF_SCRATCH + 448; //448
		cfixed16_t* data_re_out     = (cfixed16_t*)BUF_SCRATCH + 448+448;
		uint16_t noise = procLSB&0x0000FFFF;
		
		pbch_rd_dtcm( channel_est,  data_re_in,PBCH_CH_EST_SZ,PBCH_CH_EST_SZ);
		mmse_equalizer_vecalgn(channel_est,data_re_in,data_re_out,noise, 448/64);
		
		DMA_WR_COMP((uint32_t)VSPA_DATA_OUT_A_ADDR, 2*(uint32_t)data_re_out, PBCH_CH_EST_SZ);
		dma_ch_busy_wait(DMA_DDR_WR_CH);
		
		procLSB   = 0 ;
		procMSB   = (procMSB  & 0xFF000000) |  (0x1) ;
		mbox1_send(procMSB, procLSB);	
}
