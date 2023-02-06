#include "system_resources.h"
#include "all_drivers.h"
#include "definitions.h"
#include "dma.h"
#include "nr_pss.h"
#include "nr_proc_acceleration.h"

uint32_t op_mode = 800;
bool first_run = true;
uint32_t ms_ind = 0;
// -----------------------------------------------------------------------------
//! @brief          Overlay changing function.
//! @return         This function does not return.
// -----------------------------------------------------------------------------
void change_overlay(uint32_t ovl_id) {
	PROG_LOAD(ovl_mem[ovl_id]);
}

void main(void)
{

	/* At the first VSPAgo do the setup */
	if (first_run)
	{
		setup();
	    pss_init_params.pssThresh = (float)PSS_THRE;

	    sssdet_param.thre_sss = (float)SSS_THRE;  //0.1f;              //  SSS threshold value.	
	    sssdet_param.scaling  = 0.0310058593750;
	    load_cmd                = CMD_SYM_READ_AXIQ;
	    
	    enable_rx2(channel_rx);
		first_run = false;
		ext_go_enable(COMP_VSPA_GO_BIT_0, __iprd(EXT_GO_ENA));
		ext_go_enable(COMP_VSPA_GO_BIT_1, __iprd(EXT_GO_ENA));
	}
		
	mailbox_status = get_mbox_status();
	
	ms_ind = ext_go_stat_reg_read() & 0x1; 
	
	if(ms_ind)
	{
		goMSB   = (((uint32_t)CMD_MS_IND << 24)  & 0xFF000000) | 0x1;
		mbox0_send(goMSB, 0);
		ext_trig_go_clear(COMP_VSPA_GO_BIT_0, __iprd(EXT_GO_STAT));
	}
	
    if(mailbox_status &0x4)
    {
        mbox0_get(&mboxMSB, &mboxLSB);
        op_mode = (mboxMSB >> 24);
        procMSB = mboxMSB;
		procLSB = mboxLSB;
    }
	
	{	        
        process_iteration  = 0;
                
		switch (op_mode)
		{
/*		case CMD_IQDUMP:
	        mbox0_send(procMSB, procLSB);
			break;*/
			
		case CMD_RASTERSCAN:	
		case CMD_SSBSCAN:
		case CMD_SSBSCAN_CELLFOLLOW:
			uint8_t msInit = (op_mode == CMD_SSBSCAN_CELLFOLLOW) ?  1 :  0;
			process_iteration_timeout = 1200*5;
	        ssb_scan(msInit);	     
	        op_mode = NO_CMD;
			break;
			
		case CMD_CELLFOLLOW:	
			if(ms_ind)
			{			
				process_iteration_timeout = 13;
				change_overlay(1);
				dma_ch_busy_wait(DMA_PROGRAM_CH);
				
				ssb_follow();	
				
				change_overlay(0);
				dma_ch_busy_wait(DMA_PROGRAM_CH);
				op_mode = NO_CMD;
			}
			break;

		case CMD_MIB_DECODE:	
			if(ms_ind)
			{
				process_iteration_timeout = 13;
				ssb_follow_mib();	
				op_mode = NO_CMD;
			}
			break;
			
		case CMD_PBCH_DMRS_EXT:		
			change_overlay(1);
			dma_ch_busy_wait(DMA_PROGRAM_CH);
			pbch_dmrs_extraction();
			op_mode = NO_CMD;
			break;
			
	   case CMD_PBCH_LS_EST:
			pbch_ls_estimate();
			op_mode = NO_CMD;
			break;
			
	   case CMD_PBCH_EQUALISER:
		    pbch_equalizer();
			change_overlay(0);
			dma_ch_busy_wait(DMA_PROGRAM_CH);
			op_mode = NO_CMD;
			break; 
			
		default:
			//not a valid command, NACK

			break;
		}
	}
	
	DMA_CLEAR_GO_STATUS(channel_rx);
	DMA_CLEAR_GO_STATUS(DMA_DDR_WR_CH);
	__ip_write(0x8 >> 2, 0x030000F3, 0x030000E3); //CONTROL clear
	__builtin_done();
}
