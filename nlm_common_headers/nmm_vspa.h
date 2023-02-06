/*
 * SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0)
 * Copyright 2021 NXP
 */

#ifndef __NMM_VSPA_H__
#define __NMM_VSPA_H__

#define FO_CONST				(447034.8358154297F)
#define FREQ_KHZ_TO_PPB(freq)			(FO_CONST/(freq * 1000))

#define MIB_LEN (3) /* 24 bits - i.e. 3 bytes */

/* Cell search result format */
typedef struct {
	/* PSS part */
	/* value of the peak of "xcn", normalized xcorr_squared (from pss) */
	float peak_val_pss;
	/* DMEM pointer of 1st sample of PSS symbol */
	uint32_t ptrPssData;
	/* physical cell id: ID2 = {0,1,2} */
	uint32_t id2;
	/* NCO base frequency (1's complement) for the freq hypothesis
	 * detected from PSS
	 */
	int nco_freqhypo;
	float rsrp;
	float rsrq;
	/* DMEM pointer to CP mid-point of PSS symbol */
	uint32_t ptrTO;
	/* NCO base frequency (1's complement) to correct CFO */
	int nco_psscfo;
	/* RSSI of last 128 samples of recent B samples: Moving summation
	 * of squared 128 recent samples.
	 */
	float rssi_pss;

	/* SSS part */
	/* RSSI of equalized SSS (127x1 vector): Summation of squared
	 * 127 points
	 */
	float rssi_sss;
	/* value of the peak (from sss) */
	float peak_val_sss;
	/* physical cell id: ID1 = {0,1,...,335} */
	uint32_t id1;
	/* subframe flag: 0 if SF 0, 1 if SF 5 */
	uint32_t sf_flag;
	/* physical cell id: ID1*336+ID2 */
	uint32_t cellid;
	/* RSSI measured on CSRS symbols */
	float rssi_csrs;
	float rsrpAccRe;
	float rsrpAccIm;
	uint32_t cell_found;
} ssdet_out_T;

/* VSPA-shared memory control information */
#define NMM_NAME_LEN			32
#define NMM_VSPA_REGIONS_MAX		32

enum nmm_vspa_mem_type {
	VSPA_MEM_CTRL = 0,
	VSPA_MEM_OVERLAY,
	VSPA_MEM_TBL,
	VSPA_MEM_LARGE_DATA,
	VSPA_MEM_PROCESSING_OUT,
	VSPA_MEM_MAX,
};

enum nmm_vspa_tbl_type {
	VSPA_TBL_PSSDET_REF_TD = 0,
	VSPA_TBL_PSS_REF_XCORR,
	VSPA_TBL_MAX,
};

enum nmm_vspa_overlay_type {
	VSPA_OVL_TOOLS = 0,
	VSPA_OVL_CELL_SEARCH1,
	VSPA_OVL_CELL_SEARCH2,
	VSPA_OVL_CELL_FOLLOW1,
	VSPA_OVL_CELL_FOLLOW2,
	VSPA_OVL_MIB_DECODE,
	VSPA_OVL_FIRST,
	VSPA_OVL_SECOND,
	VSPA_OVL_MAX,
};

struct nmm_vspa_mem_region {
	char name[NMM_NAME_LEN];
	uint32_t type;	/* enum nmm_vspa_mem_type */
	uint32_t id;
	uint32_t offset;
	uint32_t size;
} __attribute__((__packed__));

struct nmm_vspa_mem_ctrl {
	uint32_t num_regions;
	uint32_t end;	/* Offset from start of scratch buffer */
	/* Reserve some room for future control info */
	uint32_t reserved[2];
	struct nmm_vspa_mem_region region[NMM_VSPA_REGIONS_MAX];
} __attribute__((__packed__));

#endif
