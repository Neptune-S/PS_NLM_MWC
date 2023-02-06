/*
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 * Copyright 2021-2022 NXP
 */

#ifndef __NMM_HOST_IF__
#define __NMM_HOST_IF__

/* BBDEV queues */
#define BBDEV_IPC_DEV_ID_0              0
#define BBDEV_IPC_H2M_QUEUE		0
#define BBDEV_IPC_M2H_QUEUE		1

enum nmm_msg_type {
	/* Debug ops */
	NMM_MSG_RAW = 1,
	NMM_RX_CAPTURE,
	NMM_DEC_CAPTURE,
	NMM_INT_TEST_ENABLE,
	NMM_INT_TEST_DISABLE,
	NMM_VSPA_MEM_INIT,
	NMM_FILL_MEM_CUSTOM,
	/* NMM FAPI ops */
	NMM_STOP,
	NMM_CELL_SEARCH,
	NMM_CELL_FOLLOW,
	NMM_DO_MIB,
	NMM_DO_SIB,
	NMM_TIME_UPDATE,
	NMM_DL_DUMP,
	NMM_RESET,
	NMM_IPC_RESET,
	NMM_SSB_SCAN,
	NMM_BAND_SCAN_N77,
	NMM_CELL_FOLLOW_MIB,
	NMM_CELL_FOLLOW_STOP,
	NMM_CELL_FOLLOW_MIB_INFO,
	NMM_CELL_FOLLOW_SIB_1,
	NMM_MAX_OPS,
};

enum nmm_status {
	NMM_STATUS_OK = 2000,
	NMM_STATUS_ERR,
	NMM_STATUS_TIMEOUT,
	NMM_STATUS_LOST_SYNC,
};

enum nmm_rf_band {
	NMM_RF_BAND_3 = 100,
	NMM_RF_BAND_13,
	NMM_RF_BAND_77,
	NMM_RF_BAND_NO_CHANGE,
};

/* TODO: Update if extra params used for any command! */
#define NMM_MSG_PARAMS_MAX_SIZE		0

typedef struct {
    uint16_t systemFrameNumber;
	uint16_t sfn;
	uint8_t spare;
    uint8_t half_frame_bit;
    uint8_t k_ssb_msb;
    uint8_t ssb_subcarrierOffset;
    uint8_t pdcch_configSIB1;
	uint8_t dmrs_typeA_Position;
    uint8_t cellBarred;
    uint8_t intraFreqReselection;
	uint8_t subCarrierSpacingCommon;
}mib_info_t;

struct nmm_msg {
	uint32_t type;		/* enum nmm_msg_type */
	uint32_t status;	/* enum nmm_status, valid for responses only */
	uint32_t addr;
	uint32_t size;
	/* TODO: use the extra params? */
	uint32_t band;
	uint32_t freq_khz;
	uint32_t cell_id;
	int32_t  freq_off;
	uint32_t update_int;
	uint32_t sfn;
	uint32_t si_n;
	uint32_t si_w;
	uint32_t si_T;
	uint16_t gscn;
	mib_info_t mib_info;
	/* Not used for current set of commands/responses */
	//uint32_t extra_params[NMM_MSG_PARAMS_MAX_SIZE];
}__attribute__((packed));




void ps_nr_mib_parsing(uint32_t pbchPaylod, mib_info_t * mib_info);



#define NMM_MSG_SIZE	(sizeof(struct nmm_msg))

#endif /* __NMM_HOST_IF__ */
