/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021-2022 NXP
 */

#ifndef __NMM_LIB_H
#define __NMM_LIB_H

#define NMM_LIB_VERSION "1.1.0"

/*
 * NMM Library public interface
 * @Reference: SCF224 5G FAPI: Network Monitor Mode API
 */

/*** FAPI interface ***/

/* Message types */
#define RSSI_REQUEST					(0x0200)
#define RSSI_RESPONSE					(0x0201)
#define RSSI_INDICATION					(0x0202)
#define CELL_SEARCH_REQUEST				(0x0203)
#define CELL_SEARCH_RESPONSE				(0x0204)
#define CELL_SEARCH_INDICATION				(0x0205)
#define BROADCAST_DETECT_REQUEST			(0x0206)
#define BROADCAST_DETECT_RESPONSE			(0x0207)
#define BROADCAST_DETECT_INDICATION			(0x0208)
#define SYSTEM_INFORMATION_SCHEDULE_REQUEST		(0x0209)
#define SYSTEM_INFORMATION_SCHEDULE_RESPONSE		(0x020a)
#define SYSTEM_INFORMATION_SCHEDULE_INDICATION  	(0x020b)
#define SYSTEM_INFORMATION_REQUEST			(0x020c)
#define SYSTEM_INFORMATION_RESPONSE			(0x020d)
#define SYSTEM_INFORMATION_INDICATION			(0x020e)
#define NMM_STOP_REQUEST				(0x020f)
#define NMM_STOP_RESPONSE				(0x0210)
#define CELL_FOLLOW_REQUEST				(0x0220)
#define CELL_FOLLOW_RESPONSE				(0x0221)
#define CELL_FOLLOW_INDICATION				(0x0222)
#define SSB_SCAN_REQUEST				(0x0223)
#define SSB_SCAN_RESPONSE				(0x0224)
#define BAND_SCAN_N77_REQUEST                (0X0225)
#define BAND_SCAN_N77_RESPONSE               (0x0226)
#define CELL_FOLLOW_MIB_REQUEST         (0x0227)
#define CELL_FOLLOW_MIB_RESPONSE        (0x0228)
#define CELL_FOLLOW_STOP_REQUEST       (0x0229)
#define CELL_FOLLOW_STOP_RESPONSE      (0x022a)
#define CELL_FOLLOW_SIB_1_REQUEST              (0x022b)
#define CELL_FOLLOW_SIB_1_RESPONSE              (0x022c)
/* RAT types */
#define LTE_RAT				(0) /* 4G */
#define UTRAN_RAT			(1) /* 3G */
#define GERAN_RAT			(2) /* 2G */
#define NB_IOT_RAT			(3) /* 4G IoT */
#define NR_RAT				(4) /* 5G */

/* Tags */
#define LTE_RSSI_REQUEST_TAG				(0x3000)
#define UTRAN_RSSI_REQUEST_TAG				(0x3001)
#define GERAN_RSSI_REQUEST_TAG				(0x3002)
#define RSSI_INDICATION_TAG				(0x3003)
#define LTE_CELL_SEARCH_REQUEST_TAG			(0x3004)
#define UTRAN_CELL_SEARCH_REQUEST_TAG			(0x3005)
#define GERAN_CELL_SEARCH_REQUEST_TAG			(0x3006)
#define LTE_CELL_SEARCH_INDICATION_TAG			(0x3007)
#define UTRAN_CELL_SEARCH_INDICATION_TAG		(0x3008)
#define GERAN_CELL_SEARCH_INDICATION_TAG		(0x3009)
#define PNF_CELL_SEARCH_STATE				(0x300a)
#define LTE_BROADCAST_DETECT_REQUEST_TAG		(0x300b)
#define LTE_BROADCAST_DETECT_INDICATION_TAG		(0x300e)
#define LTE_SYSTEM_INFORMATION_SCHEDULE_REQUEST_TAG	(0x3011)
#define LTE_SYSTEM_INFORMATION_REQUEST_TAG		(0x3014)
#define LTE_SYSTEM_INFORMATION_INDICATION_TAG		(0x3018)

#define LTE_CELL_FOLLOW_REQUEST_TAG			(0x4000)
#define LTE_CELL_FOLLOW_INDICATION_TAG			(0x4001)

/* Error codes */
#define MSG_OK				(100)
#define MSG_INVALID_STATE		(101)
#define MSG_INVALID_CONFIG		(102)
#define MSG_RAT_NOT_SUPPORTED		(103)
#define MSG_NMM_STOP_OK			(200)
#define MSG_NMM_STOP_IGNORED		(201)
#define MSG_NMM_STOP_INVALID_STATE	(202)
#define MSG_PROCEDURE_COMPLETE		(300)
#define MSG_PROCEDURE_STOPPED		(301)
#define MSG_PARTIAL_RESULTS		(302)
#define MSG_TIMEOUT			(303)

/* Cell Follow Errors */
#define MSG_CELL_NOT_FOUND		(400)
#define MSG_LOST_SYNC			(401)

/* NMM FAPI structures */
typedef struct fapi_request {
	uint32_t message_type;
	uint32_t rat_type;
	uint32_t* message_body;
} fapi_request_t;

typedef struct fapi_request_ssb{
	uint32_t message_type;
}fapi_request_ssb_t;

typedef struct fapi_request_nr{
	uint32_t message_type;
	uint32_t* message_body;
}fapi_request_nr_t;

typedef struct fapi_response {
	uint32_t message_type;
	uint32_t error_code;
} fapi_response_t;

typedef struct fapi_indication {
	uint32_t message_type;
	uint32_t error_code;
	uint32_t* message_body;
} fapi_indication_t;


typedef struct{
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
}stats_t;

typedef struct{
	int32_t cell_rssi_dBm;		
	int32_t cell_snr_dB;
	int32_t cfo_ppb;
	uint16_t cell_follow_cell_id;
	uint16_t cell_search_cell_id;
	uint32_t n_missed_detection;
 	uint32_t n_false_alarm;
 	uint32_t n_rx_ssb;
	uint32_t n_rx_mib;
	uint16_t n_mib_crc_fail;
	uint16_t gnb_sfn;
	uint8_t  gnb_sf;
    int32_t t_delta_minor;
	uint16_t process_itr;
	uint16_t cell_tracking;
}cell_follow_stats_t;

stats_t stats;
cell_follow_stats_t cell_follow_stats;



#define MAX_CARRIER_LIST		32
#define MAX_PCI_LIST			32
#define MAX_PSC_LIST			32
#define MAX_ARFCN_LIST			128
#define MAX_MIB_SIZE			3
#define MAX_SIB_SIZE			8192
#define MAX_SI				32


/* LTE specific structures */
typedef struct lte_cell_search_request {
	uint16_t tag;
	uint16_t length;
	uint16_t earfcn;
	uint8_t bandwidth;
	uint8_t exhaustive_search;
	uint32_t timeout;
	uint8_t num_pci;
	uint16_t pci[MAX_PCI_LIST];
} lte_cell_search_request_t;

typedef struct lte_cell_info {
	uint16_t pci;
	uint8_t rsrp;
	uint8_t rsrq;
	int16_t	freq_offset;
} lte_cell_info_t;

typedef struct lte_cell_search_indication {
	uint16_t tag;
	uint16_t length;
	uint16_t num_cells;
	lte_cell_info_t cell_info[MAX_PCI_LIST];
} lte_cell_search_indication_t;

typedef struct lte_cell_follow_request {
	uint16_t tag;
	uint16_t length;
	uint16_t earfcn;
	uint16_t pci;
	int16_t freq_offset;
	uint16_t update_interval;
} lte_cell_follow_request_t;

typedef struct nr_cell_follow_request {
	uint16_t gscn;
}nr_cell_follow_request_t;

typedef struct lte_bcast_detect_request {
	uint16_t tag;
	uint16_t length;
	uint16_t earfcn;
	uint16_t pci;
	uint32_t timeout;
} lte_bcast_detect_request_t;

typedef struct lte_bcast_detect_indication {
	uint16_t tag;
	uint16_t length;
	uint8_t  num_tx;
	uint16_t mib_len;
	uint32_t sfn_offset;
	uint8_t  mib[MAX_MIB_SIZE];
} lte_bcast_detect_indication_t;

typedef struct lte_sys_info_sched_request {
	uint16_t tag;
	uint16_t length;
	uint16_t earfcn;
	uint16_t pci;
	uint16_t dl_bw;
	uint8_t  phich_cfg;
	uint8_t  num_tx;
	uint8_t  retry_count;
	uint32_t timeout;
} lte_sys_info_sched_request_t;

typedef struct lte_sys_info_request {
	uint16_t tag;
	uint16_t length;
	uint16_t earfcn;
	uint16_t pci;
	uint16_t dl_bw;
	uint8_t  phich_cfg;
	uint8_t  num_tx;
	uint8_t  num_si_periodicity;
	uint8_t  si_periodicity[MAX_SI];
	uint8_t  si_index[MAX_SI];
	uint8_t  si_window_len;
	uint32_t timeout;
} lte_sys_info_request_t;

typedef struct lte_sys_info_indication {
	uint16_t tag;
	uint16_t length;
	uint8_t  sib_type;
	uint16_t sib_len;
	uint8_t  sib[MAX_SIB_SIZE];
} lte_sys_info_indication_t;

typedef struct {
    uint msg_type;
    uint gscn;
}parse_t;

/* FAPI request API */
int nmm_fapi_send_request(fapi_request_t *req);
//int server_init ();


/*** Debug interface ***/

/* Message types */
#define NMM_DBG_RAW_DATA			0xA00
#define NMM_DBG_RX_CAPTURE_REQUEST		0xA02
#define NMM_DBG_RX_CAPTURE_RESULT		0xA03
#define NMM_DBG_TEST_MODE_ENABLE_REQUEST	0xA04
#define NMM_DBG_TEST_MODE_ENABLE_RESULT		0xA05
#define NMM_DBG_TEST_MODE_DISABLE_REQUEST	0xA06
#define NMM_DBG_TEST_MODE_DISABLE_RESULT	0xA07
#define NMM_DBG_DECIMATOR_CAPTURE_REQUEST	0xA08
#define NMM_DBG_DECIMATOR_CAPTURE_RESULT	0xA09
#define NMM_DBG_FORCE_RESET_REQUEST		0xA0A
#define NMM_DBG_FORCE_RESET_RESULT		0xA0B
#define NMM_DBG_CMD_FILL_MEM_CUSTOM_REQUEST		0xA0C
#define NMM_DBG_CMD_FILL_MEM_CUSTOM_RESULT		0xA0D

#define NMM_FILENAME_SIZE			32

/* Debug structures */
typedef struct nmm_dbg_request {
	uint32_t type;
	uint32_t *body;
} nmm_dbg_request_t;

typedef fapi_response_t nmm_dbg_response_t;

typedef struct nmm_rx_capture_request {
	uint16_t earfcn;
	uint32_t length;
	char filename[NMM_FILENAME_SIZE];
} nmm_rx_capture_request_t;

typedef struct nmm_fill_mem_custom_request {
	char filename[NMM_FILENAME_SIZE];
} nmm_fill_mem_custom_request_t;

typedef struct nmm_test_mode_enable_request {
	char filename[NMM_FILENAME_SIZE];
} nmm_test_mode_enable_request_t;

/* Debug request API */
int nmm_dbg_send_request(nmm_dbg_request_t *req);

/* User callbacks for passing FAPI & debug response */
struct nmm_cb {
	/* FAPI */
	void (*fapi_receive_response)(fapi_response_t *rsp);
	void (*fapi_receive_indication)(fapi_indication_t *ind);
	/* Debug */
	void (*dbg_receive_response)(nmm_dbg_response_t *rsp);
};

/* Library initialization */
int nmm_init(struct nmm_cb *cbs);

/* Library cleanup */
void nmm_free(void);

#endif /* NMM_LIB_H */
