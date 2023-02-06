/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017-2018, 2021 NXP
 * All rights reserved.
 */

#ifndef __NMM_APP_H__
#define __NMM_APP_H__

#include <la9310.h>
#include "nmm_host_if.h"
#include "pbch.h"
#include "cmd_ctrl_vspa_intfce.h"
#include "la93xx_bbdev_ipc.h"


#define MAX_GSCN                (619)
#define FS_BY_TWO_POWER_32      (0.003576278)
#define PRINT_CONSOLE           (1)
#define DDR_SSDET_OUT_ADDRESS   (0xA0219100)
#define NCO_FREQ_KHZ_CONVERTOR  (15360000 / 4294967295) //(15.36MSPS/2^32)
#define VSPA_SCALING_FACTOR     (1)
#define SYMBOL_SIZE             (548) //512+36
#define BASEBAND_SAMPLING_RATE  (15360000)
#define PPS_COUNT_20_MS	        (1228800)	
#define FILL_MEM_DDR_WR_ADDRESS (0xA0219100 + 0x80000)
#define PPB_3GPP_LIM            (90)
#define INVALID_SSB_IDX			(10)
#define N77_BAND_GSCN_START		(7711)
#define ENQUEUE_RETRIES         (100) 
#define GET_BUF_RETRIES         (10)
#define RSSI_OFFSET_DB          (8) // This is a temporary fix to the variation w.r.t VGA  
/* Filter Alphas */
#define ALPHA_CFO   (0.0001f)
#define ALPHA_NOISE (0.04f)

#define nmm_do_print(f, fmt, ...)                    \
	{                                                \
		unsigned long msec = get_timestamp_msec();   \
		f("[%05d.%03d] ", msec / 1000, msec % 1000); \
		f("nmm_app: %s: ", __func__);                \
		f(fmt, ##__VA_ARGS__);                       \
	}


#if NMM_APP_LOGGING == 0
/* Default: nmm_print() shows up on console, nmm_print_dbg() in mem log only */
#define nmm_print(fmt, ...) \
	nmm_do_print(nmm_log_to_console, fmt, ##__VA_ARGS__)
#define nmm_print_dbg(fmt, ...) \
	nmm_do_print(nmm_log_to_mem, fmt, ##__VA_ARGS__)
#define hexdump(buf, len) \
	nmm_do_hexdump(nmm_log_to_mem, buf, len)
#elif NMM_APP_LOGGING == 1
/* Debug: both nmm_print() and nmm_print_dbg() show up on console */
#define nmm_print(fmt, ...) \
	nmm_do_print(nmm_log_to_console, fmt, ##__VA_ARGS__)
#define nmm_print_dbg nmm_print
#define hexdump(buf, len) \
	nmm_do_hexdump(nmm_log_to_console, buf, len)
#elif NMM_APP_LOGGING == 2
/* Silent: nmm_print() writes in mem log only, nmm_print_dbg() does nothing */
#define nmm_print(fmt, ...) \
	nmm_do_print(nmm_log_to_mem, fmt, ##__VA_ARGS__)
#define nmm_print_dbg(fmt, ...)
#define hexdump(buf, len)
#endif /* NMM_APP_LOGGING */



typedef int (*print_func)(const char *fmt, ...);
__attribute__((unused)) static void nmm_do_hexdump(print_func f, uint8_t *buf, uint32_t len)
{
	int i;

	f("Dump @%p[%d]:", buf, len);
	for (i = 0; i < len; i++)
	{
		if (i % 16 == 0)
			f("\r\n%08x: ", i);
		f("%02x ", buf[i]);
	}

	f("\r\n");
}

#define NO_TIMEOUT (-1)
#define VSPA_IMM_RESPONSE 0

#ifdef NMM_APP_VSPA_TIMER
#define VSPA_ACK_TIMEOUT_MS 10
#define VSPA_CELL_SEARCH_TIMEOUT_MS 100
#define VSPA_RX_CAPTURE_TIMEOUT_MS 100
#define VSPA_DEC_CAPTURE_TIMEOUT_MS (VSPA_RX_CAPTURE_TIMEOUT_MS * 32)
#define VSPA_UPDATE_TIMEOUT_MS NO_TIMEOUT
#define VSPA_DUMP_TIMEOUT_MS NO_TIMEOUT /* TBD */
#define VSPA_CF_UPDATE_TIMEOUT_MS(ui) (10 * 2 * (ui))
#else
#define VSPA_ACK_TIMEOUT_MS 0
#define VSPA_CELL_SEARCH_TIMEOUT_MS NO_TIMEOUT
#define VSPA_RX_CAPTURE_TIMEOUT_MS NO_TIMEOUT
#define VSPA_RX_CAPTURE_TIMEOUT_MS NO_TIMEOUT
#define VSPA_UPDATE_TIMEOUT_MS NO_TIMEOUT
#define VSPA_DUMP_TIMEOUT_MS NO_TIMEOUT
#define VSPA_CF_UPDATE_TIMEOUT_MS(ui) NO_TIMEOUT
#endif

#define VSPA_MEM_ALIGN 64
#define VSPA_OVERLAY_MAX_SIZE (VSPA_OVL_MEM_SIZE >> 1)
#define VSPA_DATA_BLOCK_SIZE 4096
#define NUM_BLOCKS(bytes)             \
	((bytes) / VSPA_DATA_BLOCK_SIZE + \
	 (((bytes) % VSPA_DATA_BLOCK_SIZE) ? 1 : 0))

typedef void (*RxProcessFn)(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *req);


#define MAX_FREQ_THRES 1000 /* 1kHz*/

typedef struct
{
	int32_t r_real;
    int32_t r_imag;
	float cfo_est;
	float angle_est;
} iir_filter_cfo_param_t;

typedef struct
{
	float r;
} iir_filter_noise_param_t;

typedef struct 
{   
	float  cell_rssi_dBm;		
	float  cell_noise_dBm;
	float  cell_snr_dB;
	float  cell_rssi;
	float  cell_noise; 
	uint16_t cell_search_cell_id;
	uint16_t cell_follow_cell_id;
	uint16_t gscn;
}cell_acquistion_response_t;

typedef struct 
{
 	uint32_t n_missed_detection;
 	uint32_t n_false_alarm;
	 /* More can be added here */
 	uint32_t n_rx_ssb;
	uint32_t n_rx_mib;
	uint32_t n_mib_crc_fail;
}ssb_stats_t;


typedef struct {
	uint32_t   cellId;
	uint32_t   rssi;
	int32_t    cfo;
	uint32_t   process_itration;
	int32_t    corr_real;
	int32_t    corr_imag;	
	uint32_t   noise_pwr;
	uint32_t   t0;
	uint32_t   t_delta_minor;
	uint32_t   t_delta_major;		//can be +1 ,+2...
    bool       valid;
} vspa_controller_if_t;

typedef struct{
	uint64_t t0_update;
	uint32_t t_delta_minor_last;
	uint32_t t_delta_minor_latest;
	int32_t t_delta_minor;
	uint32_t mib_proc_delay;
	uint32_t pps_adjust;
	uint16_t ue_sfn;
	uint16_t gnb_sfn;
	uint16_t tti_cnt;
	uint16_t mib_scheduled_tti_cnt;
	uint16_t mib_scheduled_sfn;
	uint16_t process_itr;
	uint8_t  ue_sf;
	uint8_t  gnb_sf;
	uint8_t  gnb_sym;
	uint8_t  schedule_mib ;
	uint8_t  mib_crc_flag;
	uint8_t  cons_msd_ssb_det;
	uint8_t  pps_timer_start;	
} schedular_params_t;


extern int32_t getCrtFreq();
extern cell_acquistion_response_t* cell_acquistion_response;
extern ssb_stats_t* ssb_stats;
extern  mib_info_t* mib_info;
extern TimerHandle_t xScheduler;
extern schedular_params_t* schedular_params;


const uint16_t gain_correction_gscn_db[MAX_GSCN] = {150, 150, 150, 150, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
 150, 150, 150, 150, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148,
 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 148, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147,
 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 145, 145,
 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145, 145,
 145, 145, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144,
 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 144, 142, 142, 142, 142, 142, 142, 142, 142, 142,
 142, 142, 142, 142, 142, 142, 142, 142, 142, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141,
 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141, 141};


void convert_to_db(float pwr, float *pwr_dBm);
void prvStartVspaTimer(uint32_t rsp_type, uint32_t timeout_ms);
void prvSchedularCallback(TimerHandle_t xTimer);
void prvSetVCXO(int32_t val);  
void prvParseCellInfo(vspa_controller_if_t* vspa_controller_if);
void vRxLoop(void *);
void vVSPALoop(void *);
void vMibLoop(void *);
void vSchedularLoop(void *);
unsigned long get_timestamp_msec(void);
int prvCalRadio();
int prvSetRadio(struct nmm_msg *req);
int vBBDEVSetup(void);

#endif /* __NMM_APP_H__ */
