#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "common.h"
#include "immap.h"
#include "la9310_main.h"
#include "la9310_irq.h"
#include "core_cm4.h"
#include "la9310_edmaAPI.h"
#include "bbdev_ipc.h"
#include "rfic_api.h"
#include "nmm_host_if.h"
#include "nmm_avi.h"
#include "sync_timing_device.h"
#include "math.h"
#include "nmm_vspa.h"
#include "rfic_sw_cmd.h"
#include "rfic_dac.h"
#include "rfic_demod.h"
#include "phytimer.h"
#include "nmm_logging.h"
#include "timers.h"
#include "cmd_ctrl_vspa_intfce.h"
#include "semphr.h"
#include "pbch.h"
#include "nmm_app.h"


static uint32_t tunefreq;
static uint32_t crt_band;
static uint32_t crt_freq;
static TimerHandle_t vspa_timer;
RficResp_t vConfigRfic(uint32_t rasterFlag);


void convert_to_db(float pwr, float *pwr_dBm)
{
	if(pwr>0)
		*pwr_dBm = 10*log2(pwr)*0.30102;
    else
	    *pwr_dBm = -100;
}

unsigned long get_timestamp_msec(void)
{
	TickType_t ticks;
	unsigned long msec;

	ticks = xTaskGetTickCount();
	msec = (unsigned long)(ticks * portTICK_PERIOD_MS);

	return msec;
}

void prvSetVCXO(int32_t val)
{
	SyncTimingDeviceContext_t *sync_device = pxSyncTimingDeviceGetContext();
	SyncStatus_t status;

	if (!sync_device)
		return;

	nmm_print_dbg("Setting VCXO to %d\r\n", val);
	status = xSyncTimingDeviceFWAPIVariableOffsetDco(sync_device, 1, val);
	if (status != eSyncStatusSuccess)
		nmm_print("Sync failed (status %d)\r\n", status);
}


int prvCalRadio()
{
	RficHandle_t pRficDev;
	int ret;

	pRficDev = xRficLibInit();
	if (!pRficDev)
	{
		nmm_print("RFIC library not initialized\r\n");
		return -1;
	}

	ret = xRficFastCal(pRficDev);
	if (ret != RFIC_SUCCESS)
	{
		nmm_print("Fast calibration command failed successfuly\r\n");
		return ret;
	}

	return 0;
}

void prvStartVspaTimer(uint32_t rsp_type, uint32_t timeout_ms)
{
	int ret;

#ifndef NMM_APP_VSPA_TIMER
	nmm_print("Oups, we shouldn't be here!\r\n");
#endif

	vTimerSetTimerID(vspa_timer, (void *)rsp_type);
	ret = xTimerChangePeriod(vspa_timer, pdMS_TO_TICKS(timeout_ms), 0);
	if (ret == pdFAIL)
	{
		nmm_print("Error updating timer period to %dms\n", timeout_ms);
		return;
	}
	xTimerStart(vspa_timer, 0);
}


RficResp_t vConfigRfic(uint32_t rasterFreq)
{
	RficHandle_t pRficDev;
	RficBand_t xRficBand;
	int gain_set = 0;
	RficResp_t ret = RFIC_SUCCESS;

	pRficDev = xRficLibInit();
	if (!pRficDev)
	{
		log_info("RFIC library not initialized\n\r");
		return -1;
	}

	xRficBand = RFIC_BAND_N77;

	if (rasterFreq == 0)
	{
		ret = xRficSetBand(pRficDev, xRficBand);
		if (ret != RFIC_SUCCESS)
		{
			log_info("SetBand failed for band %d\r\n", xRficBand);
			return ret;
		}

		log_info("RFIC Set-Band Success\n\r");

		//set gain only once
		// TODO: use proper BSP power adjustments when avaiable
		if (!gain_set)
		{
			ret = RficDemodGainCtrl(pRficDev, 0, 7);
			if (ret != RFIC_SUCCESS)
			{
				log_info("%s: RficDemodGainCtrl Failed ,ret  %d\n\r", __func__, ret);
				ret = RFIC_ERROR;
			}
			else
			{
				log_info("RficDemodGainCtrl set\r\n");
			}

			gain_set = 1;
		}

		ret = xRficAdrfGainCtrlBand77(pRficDev, 1);
		if (ret != RFIC_SUCCESS)
		{
			log_info("SetVGA Gain failed for band %d\r\n", xRficBand);
			return ret;
		}

		rasterFreq = 3500000;
	}

	tunefreq = rasterFreq + 15;
	ret = xRficAdjustPllFreq(pRficDev, tunefreq);
    tunefreq -= 15;
	if (ret != RFIC_SUCCESS)
	{
		log_info("Set freq failed for band %d\r\n", xRficBand);
		return ret;
	}

	return ret;
}

int32_t getCrtFreq()
{
	return tunefreq;
}

int prvSetRadio(struct nmm_msg *req)
{
	RficHandle_t pRficDev;
	RficBand_t xRficBand;
	static int gain_set = 0;
	int ret;

	/*
	 * Call this here to make sure VCXO is reset before each command
	 * involving RF
	 */
	if (req->freq_off)
		prvSetVCXO(req->freq_off * 10);

	pRficDev = xRficLibInit();
	if (!pRficDev)
	{
		nmm_print("RFIC library not initialized\r\n");
		return -1;
	}

	switch (req->band)
	{
	case NMM_RF_BAND_3:
		xRficBand = RFIC_BAND_B3;
		break;
	case NMM_RF_BAND_13:
		xRficBand = RFIC_BAND_B13;
		break;
	case NMM_RF_BAND_NO_CHANGE:
		/* Nothing to do here */
		return 0;
	default:
		nmm_print("Invalid RF band requested (%d)\r\n", req->band);
		return -1;
	}

	ret = xRficSetBand(pRficDev, xRficBand);
	if (ret != RFIC_SUCCESS)
	{
		nmm_print("SetBand failed for band %d\r\n", xRficBand);
		return ret;
	}

	ret = xRficAdjustPllFreq(pRficDev, req->freq_khz);
	if (ret != RFIC_SUCCESS)
	{
		nmm_print("AdjustPllFreq failed for freq %dkHz\r\n", req->freq_khz);
		return ret;
	}

	crt_band = req->band;
	crt_freq = req->freq_khz;

	//set gain only once
	// TODO: use proper BSP power adjustments when avaiable
	if (!gain_set)
	{
		ret = RficDemodGainCtrl(pRficDev, 0, 7);
		if (ret != RFIC_SUCCESS)
		{
			nmm_print_dbg("%s: RficDemodGainCtrl Failed ,ret  %d\n\r", __func__, ret);
			ret = NMM_STATUS_ERR;
		}
		else
		{
			nmm_print_dbg("RficDemodGainCtrl set\r\n");
		}

		prvDacWriteUpdate(2458, DAC1_I2C_ADDR);
		prvDacWriteUpdate(2458, DAC2_I2C_ADDR);

		nmm_print_dbg("prvDacWriteUpdate set\r\n");

		gain_set = 1;
	}

	nmm_print_dbg("RF config ok: band = %d, freq = %dkHz\r\n",
				  req->band, req->freq_khz);

	return 0;
}

