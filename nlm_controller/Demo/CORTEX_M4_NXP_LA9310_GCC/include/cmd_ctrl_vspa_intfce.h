/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2017, 2021 NXP
 */


#ifndef _CMD_CTRL_VSPA_INTFCE_H_
#define _CMD_CTRL_VSPA_INTFCE_H_

#include "rfic_api.h"
#include "la9310_avi.h"
#include "pbch.h"
#include "semphr.h"
#include "nmm_app.h"

#define INVALID_CELLID (1008)

typedef struct vspa2_complex_fixed16 {
   short real;
   short imag;
}cfixed16_t;

#define CMD_SSBSTOP	                (0)
#define CMD_SSBSCAN	                (1)
#define CMD_CELLFOLLOW              (5)
#define CMD_VSGDUMP	                (6)
#define CMD_RASTERSCAN              (13)
#define CMD_SSBSCAN_CELLFOLLOW      (14)
#define CMD_SSBSCAN_CELLFOLLOWSTOP  (15)
#define CMD_CELL_FOLLOW_MIB         (17)
#define CMD_PBCH_SEARCH_SSB_INDEX	(19)
#define CMD_PBCH_CHANNEL_ESTIMATOR	(20)
#define CMD_PBCH_CHANNEL_EQUALIZER  (21)

#define DDR_RW_ADDRESS              (0xC0000000)
#define DDR_RW_SIZE                 (0x4B0000) //49,15,200 = (61.44MSPS * 20MS) * 4
#define PPS_OUT_20_MS_COUNT         (1228800)
#define SSB_PERIOD                   (20)
#define MIB_NOT_SCHEDULE             (0)
#define MIB_ENABLE                   (1)
#define MIB_SCHEDULE                 (3)
#define MIB_PERIOD                   (80)
#define MIB_DELAY                    (125) // In the multiple of MIB period.
#define NUM_SF_IN_SFN                (10)
#define MAX_SFN_COUNT                (1024)
#define NUM_SUBCARRIER_SSS			 (127)
#define NUM_SUBCARRIER_NOISE		 (113)


extern RficResp_t vConfigRfic(uint32_t freq);
void vRegisterSampleCLICommands(void);
extern void sendStoptoVSPA();
extern volatile uint8_t mbox_rcvd;

int32_t getCrtFreq();
extern uint8_t prvGetRasterScanRsp();
extern void prvResetRasterScanRsp();
extern uint8_t pps_correction;
extern struct avi_mbox host_mbox;
extern uint16_t raster_scan_detected_ssb;


void *getAviHandle();
void sendCliCommandToVspa(uint8_t opcode);
extern volatile uint8_t scantimeout ;
extern void vResetSsbTimeOut();
extern uint8_t vGetSsbTimeOut();
extern uint8_t vGetCellFollowTimeOut();
extern void vResetCellFollowTimeOut();
extern void sendMboxtoVSPA(struct avi_mbox host_mbox);

float prvArcTan2(uint32_t im, uint32_t re);
#endif
