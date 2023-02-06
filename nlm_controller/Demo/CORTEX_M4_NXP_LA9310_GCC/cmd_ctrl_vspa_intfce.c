
#include "FreeRTOS.h"
#include "task.h"
#include "la9310_avi.h"
//#include "la9310_avi_ds.h"
#include "debug_console.h"
#include "cmd_ctrl_vspa_intfce.h"
#include "timers.h"
#include "phytimer.h"
#include "math.h"
#include "semphr.h"
#include "pbch.h"
#include "nmm_app.h"

uint8_t pps_correction = 0;
struct avi_mbox host_mbox;

uint16_t raster_scan_detected_ssb = 0;

void sendMboxtoVSPA(struct avi_mbox host_mbox)
{
    int mbox_index = 0;

    void *avihndl = getAviHandle();

    /*Sending AVI API (CM4-->VSPA )*/
    if (0 != iLa9310AviHostSendMboxToVspa(avihndl, host_mbox.msb,
                                          host_mbox.lsb, mbox_index))
    {
        log_err("ERR:avihandle 0x%x and MBOX %d Failed\n\r", avihndl, mbox_index);
    }  
}

void sendCliCommandToVspa(uint8_t cmd)
{
    log_dbg("%s: Sending CLI Command  %d to VSPA .\n\r", __func__, cmd);
    switch (cmd)
    {
    case CMD_VSGDUMP:
    case CMD_SSBSCAN:
        host_mbox.lsb = DDR_RW_ADDRESS;
        host_mbox.msb = DDR_RW_SIZE;                                          //(24-bit lsb of msb, in units of 1536 Samples)
        host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
        sendMboxtoVSPA(host_mbox);
        vTaskDelay( 100 / portTICK_PERIOD_MS ); 
	break;

    case CMD_RASTERSCAN:
        log_info("\nScanning band n77...\n\r");

        uint16_t gscnstart = 7711, gscnend = 8329;
        uint16_t nrefStart = gscnstart - 7499;
        uint32_t i;
        uint32_t freq_start_offset;
        uint16_t gscnIdx = 0;
        uint32_t gscnFreq;
        RficResp_t ret;

        host_mbox.lsb = 0;

        freq_start_offset = 3000 * 1000; //In Khz
        for (gscnIdx = nrefStart; gscnIdx <= (gscnend - 7499); gscnIdx++)
        {
            gscnFreq = freq_start_offset + (gscnIdx * 1440); //In Khz

            ret = vConfigRfic(gscnFreq);
            for (i = 0; i < 100000; i++);

            if (ret)
                log_info("Config RFIC return  %d \n\r", ret);

            host_mbox.msb = 0;
            host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
            sendMboxtoVSPA(host_mbox);
            prvResetRasterScanRsp();
		
	        vTaskDelay( 100 / portTICK_PERIOD_MS );

            if (prvGetRasterScanRsp() == 1)
            {
                prvResetRasterScanRsp();
                log_info("Status: SSB_FOUND \n\r");
            }
        }

        log_info("\nScanning Complete, Number of SSB Detected: %d\n\r", raster_scan_detected_ssb);
        raster_scan_detected_ssb = 0;
        break;

    case CMD_CELLFOLLOW:
        log_info("\nCell Follow");
        uint32_t ret1;
        uint32_t freq_start;
        uint32_t gscn_freq;
        
        log_info(" for GSCN %d in Progress...\n\r", cell_acquistion_response->gscn);

        freq_start = 3000 * 1000;
        gscn_freq = (freq_start + ((cell_acquistion_response->gscn - 7499)) * 1440); //In Khz

        vConfigRfic(gscn_freq);
        for (ret1 = 0; ret1 < 1000; ret1++) ;
       
        schedular_params->schedule_mib = MIB_NOT_SCHEDULE;  
        cmd = CMD_SSBSCAN_CELLFOLLOW;
        host_mbox.lsb = 0;
        host_mbox.msb = 0;                                          //(24-bit lsb of msb, in units of 1536 Samples)
        host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
        sendMboxtoVSPA(host_mbox);
        break;

    case CMD_SSBSCAN_CELLFOLLOWSTOP:

        vPhyTimerComparatorDisable(PHY_TIMER_COMP_VSPA_GO_0);
        vPhyTimerComparatorForce(PHY_TIMER_COMP_VSPA_GO_0, ePhyTimerComparatorOut0);
        memset (schedular_params,0,sizeof(schedular_params_t));
        vPhyTimerComparatorDisable(PHY_TIMER_COMP_PPS_OUT);
        memset(ssb_stats,0,sizeof(ssb_stats_t));
        memset(cell_acquistion_response,0,sizeof(cell_acquistion_response_t));
        prvSetVCXO(0);
        /* Redo radio calibration */
        prvCalRadio();
        log_info("Stop Command is received\n\r");
        break;

    case CMD_CELL_FOLLOW_MIB:
        log_info("\nCell Follow MIB");

        log_info(" for GSCN %d in Progress...\n\r", cell_acquistion_response->gscn);

        freq_start = 3000 * 1000;
        gscn_freq = (freq_start + ((cell_acquistion_response->gscn - 7499)) * 1440); //In Khz

        vConfigRfic(gscn_freq);
        for (ret1 = 0; ret1 < 1000; ret1++) ;

        schedular_params->schedule_mib = MIB_ENABLE;  
        cmd = CMD_SSBSCAN_CELLFOLLOW;// ;
        host_mbox.lsb = 0;
        host_mbox.msb = 0;                                          //(24-bit lsb of msb, in units of 1536 Samples)
        host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
        sendMboxtoVSPA(host_mbox);
        break;    

    default:
        break;
    }
}


