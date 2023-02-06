// =============================================================================
//! @file           capture.c
//! @brief          Functions used to acquire data for processing.
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

#include "all_drivers.h"
#include "system_resources.h"
#include "definitions.h"
#include "dma.h"
#include "nr_pss.h"

#define NMM

__attribute__(( section(".scratch") ))
__attribute__((aligned(128 * UPHW)))	vspa_controller_if_t ssb_info;

uint32_t* p_ssb_info_structure = (uint32_t*)&ssb_info;
size_t ssb_info_size           = sizeof(vspa_controller_if_t) << 1;

__attribute__(( section(".text.ssb")))
void ssb_scan(uint8_t msInit)
{		
	uint32_t ret = 0; 
	uint32_t iFH = 2; 
		
#ifdef STIMULUS	
	veccpy(CBUF_PROC, BUF_SCRATCH, 64);
	memsetvec(&BUF_SCRATCH[0], 0x00, 64);
#endif	
		
	iq_capture(load_cmd, dma_load_addr);
	dma_ch_busy_wait(channel_rx);
	
	ssb_info.t0 =  read_phytmr();
	
	iq_capture(load_cmd, dma_load_addr);
	
	while(process_iteration < process_iteration_timeout)
	{   
		
		dma_ch_busy_wait(channel_rx);
		
		decimator();

		iq_capture(load_cmd, dma_load_addr);
	
		ret  = cell_search(iFH); 
	
        if((process_iteration>0) && ((process_iteration%SSB_ITER_PER_FREQ_HYPO)==0)) //This is scanning for 40ms per hypothesis 
        {	
        	iFH += 1;
        	iFH %= N_PSS_FREQ_HYPO; 
        }
        
		if(ret == SSB_DETECTED)	
		{   
		//	curr_read = read_phytmr();
		//	procLSB   = curr_read ;
			if(msInit == 1)
			{
				goMSB   = (((uint32_t)CMD_MS_START << 24)  & 0xFF000000) | 0x1;
				goLSB   = ssb_info.t0;
				mbox0_send(goMSB, goLSB);
			}
			
			procLSB   = 0;
			procMSB   = (procMSB  & 0xFF000000) | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF) | (ret & 0x1) ;
			mbox1_send(procMSB, procLSB);
			
			ssdet_out.cellid 		  =  3*ssdet_out.id1 + ssdet_out.id2; 
			ssb_info.cellId           =  ssdet_out.cellid; 
			ssb_info.process_itration =  process_iteration;
			ssb_info.cfo              =  ssdet_out.nco_psscfo;
			ssb_info.rssi             =  (uint32_t)(ssdet_out.rssi_sss * 1000);
			ssb_info.noise_pwr        =  (uint32_t)(ssdet_out.noise_est * 1000);
			ssb_info.corr_real        =  (int32_t)(ssdet_out.track_ffo_corr_re * 100000);
			ssb_info.corr_imag        =  (int32_t)(ssdet_out.track_ffo_corr_im * 100000);
			ssb_info.valid            =  1;
								
			DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
			dma_ch_busy_wait(DMA_DDR_WR_CH);
			
			break;
		}		
		process_iteration++;
	}
	
	if(process_iteration >= process_iteration_timeout)
	{   
		ret       = SSB_SCAN_TIMEOUT;
	   
		procLSB   = 0 ;
		procMSB = procMSB | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF);
	    procMSB = (procMSB & 0xFFFFFFFE) | (ret & 0x1); 
		mbox1_send(procMSB, procLSB);	
	
		ssb_info.valid            =  0;
					
		DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
		dma_ch_busy_wait(DMA_DDR_WR_CH);		
	}	
}

__attribute__(( section(".text.cf")))
void ssb_follow(void)
{		
	uint32_t ret = 0; 
	
	iq_capture(load_cmd, dma_load_addr);
	
	while(process_iteration < process_iteration_timeout)
	{   
		
		dma_ch_busy_wait(channel_rx);

		decimator();

		iq_capture(load_cmd, dma_load_addr);
	
		ret  = cell_follow(); 
	        
		if(ret == SSB_DETECTED)	
		{   
			curr_read =  read_phytmr();
			procLSB   = curr_read ;
			procMSB   = (procMSB  & 0xFF000000) | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF) | (ret & 0x1) ;
			mbox1_send(procMSB, procLSB);
			ssdet_out.cellid 		  =  3*ssdet_out.id1 + ssdet_out.id2; 
			ssb_info.cellId           =  ssdet_out.cellid; 
			ssb_info.process_itration =  process_iteration;
			ssb_info.cfo              =  -ssdet_out.nco_psscfo;
			ssb_info.rssi             =  (uint32_t)(ssdet_out.rssi_sss * 1000);
			ssb_info.noise_pwr        =  (uint32_t)(ssdet_out.noise_est * 1000);
			ssb_info.corr_real        =  (int32_t)(ssdet_out.track_ffo_corr_re * 10000);
			ssb_info.corr_imag        =  (int32_t)(ssdet_out.track_ffo_corr_im * 10000);
			ssb_info.valid            =  1;
			
			DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
			dma_ch_busy_wait(DMA_DDR_WR_CH);
			
			break;
		}		
		process_iteration++;
	}
	
	if(process_iteration >= process_iteration_timeout)
	{   
		curr_read = read_phytmr();
		ret = SSB_SCAN_TIMEOUT;
		procLSB = curr_read;
		procMSB = (procMSB  & 0xFF000000) | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF) | (ret & 0x1) ;
		mbox1_send(procMSB, procLSB);	
		
		ssb_info.valid            =  0;

		DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
		dma_ch_busy_wait(DMA_DDR_WR_CH);
		
	}	
}


__attribute__(( section(".text.ssb")))
void ssb_follow_mib(void)
{		
	uint32_t ret = 0; 
	
	iq_capture(load_cmd, dma_load_addr);
	
	while(process_iteration < process_iteration_timeout)
	{   
		
		dma_ch_busy_wait(channel_rx);

		decimator();

		iq_capture(load_cmd, dma_load_addr);
	
		ret  = cell_follow_mib(); 
			
	        
		if(ret == PBCH_CAPTURED)	
		{   
			curr_read =  read_phytmr();
			procLSB   = (uint32_t)p_SSB_PBCH_SYM ;
			procMSB   = (procMSB  & 0xFF000000) | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF) | (ret & 0x1) ;
			mbox1_send(procMSB, procLSB);
			ssdet_out.cellid 		  =  3*ssdet_out.id1 + ssdet_out.id2; 
			ssb_info.cellId           =  ssdet_out.cellid; 
			ssb_info.process_itration =  process_iteration;
			ssb_info.cfo              =  -ssdet_out.nco_psscfo;
			ssb_info.rssi             =  (uint32_t)(ssdet_out.rssi_sss * 1000);
			ssb_info.valid            =  1;
			
			DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
			dma_ch_busy_wait(DMA_DDR_WR_CH);
			
			break;
		
		}		
		process_iteration++;
	}
	
	if(process_iteration >= process_iteration_timeout)
	{   
		curr_read = read_phytmr();
		ret = SSB_SCAN_TIMEOUT;
		procLSB = curr_read;
		procMSB = (procMSB  & 0xFF000000) | (((uint16_t)(process_iteration) << 1) & 0x0001FFFF) | (ret & 0x1) ;
		mbox1_send(procMSB, procLSB);	
		
		ssb_info.valid            =  0;

		DMA_WR(DTCM_CMD_RSP_ADDRESS, 2*(uint32_t)p_ssb_info_structure, ssb_info_size);
		dma_ch_busy_wait(DMA_DDR_WR_CH);
				
	}	
}

