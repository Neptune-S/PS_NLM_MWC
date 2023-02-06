/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2021-2022 NXP
 * All rights reserved.
 */

#ifndef __NMM_AVI_H__
#define __NMM_AVI_H__

#include <types.h>
#include "FreeRTOS.h"

/* Mailboxes */
#define NMM_OPS_MBOX_ID			0
#define NMM_CTRL_MBOX_ID		1

/* Opcodes for mailbox 0 (NLM interface) */
#define NMM_OP_NMM_STOP			0
#define NMM_OP_CELL_SEARCH		1
#define NMM_OP_MIB_PROCEDURE		2
#define NMM_OP_SIB_PROCEDURE		3
#define NMM_OP_RSSI_CALCULATE		4
#define NMM_OP_CELL_FOLLOW		5
#define NMM_DBG_RX_CAPTURE		6
#define NMM_DBG_DEC_CAPTURE		7
#define NMM_OP_TIME_UPDATE		8
#define NMM_OP_MIB_DUMP			9
#define NMM_OP_SI_DUMP			10
//#define NMM_DBG_FILL_MEM_CUSTOM  11
/* Opcodes for mailbox 0 (debug & test interface) */
#define NMM_DBG_INT_TEST_ENABLE		11
#define NMM_DBG_INT_TEST_DISABLE	12
#define NMM_OP_RASTER_SCAN			13
#define NMM_OP_CELL_FOLLOW_CELL_SEARCH          (14)
#define NMM_CTRL_CELL_FOLLOW_MIB	        	(17)
#define NMM_CTRL_PBCH_SEARCH_SSB_INDEX		    (19)
#define NMM_CTRL_PBCH_CHANNEL_ESTIMATOR			(20)
#define NMM_CTRL_PBCH_CHANNEL_EQUALIZER		    (21)
#define NMM_CTRL_MS_START					    (22)
#define NMM_CTRL_MS_IND						    (23)

/* Opcodes for mailbox 1 (VSPA control interface) */
#define NMM_CTRL_CELL_FOLLOW		NMM_OP_CELL_FOLLOW
#define NMM_CTRL_CELL_SEARCH		NMM_OP_CELL_SEARCH
#define NMM_CTRL_MIB_PROCEDURE		NMM_OP_MIB_PROCEDURE
#define NMM_CTRL_SIB_PROCEDURE		NMM_OP_SIB_PROCEDURE
#define NMM_CTRL_RX_CAPTURE			NMM_DBG_RX_CAPTURE
#define NMM_CTRL_DEC_CAPTURE		NMM_DBG_DEC_CAPTURE
#define NMM_CTRL_RASTER_SCAN		NMM_OP_RASTER_SCAN	
#define NMM_CTRL_CELL_FOLLOW_CELL_SEARCH NMM_OP_CELL_FOLLOW_CELL_SEARCH


#define NMM_ACK_AUTO_REPLY		10


/* Value for ctrl.param1 in message from VSPA */
#define NMM_MSG_TO_HOST			0xFFFFFF
/* Values for param2 in message from VSPA */
#define NMM_LOST_SYNC			0
#define NMM_REQUEST_SFN_DECODE		1

struct nmm_mbox{
	/* MSB [opcode | param 1] */
	/* MSB [opcode | interval | cell_id ] */
	union {
		uint32_t msb;
		struct nmm_mbox_ctrl {
			uint32_t param1:24;
			uint32_t opcode:8;
		} ctrl;
		struct nmm_mbox_cmd {
			uint32_t cell_id:9;
			uint32_t interval:15;
			uint32_t opcode:8;
		} cmd;
	};
	/* LSB [ param2   ] */
	/* LSB [ out_addr ] */
	union {
		uint32_t lsb;
		uint32_t param2;
		uint32_t out_addr;
		struct nmm_mbox_status {
			uint32_t ack:1;
			uint32_t err_code:8;
			uint32_t unused:23;
		} status;
	};
}__attribute__((packed));

/* Send message to VSPA mbox_id */
int vNMMMbxSend(struct nmm_mbox *h2v, uint32_t mbox_id);
/*
 * Receive message from VSPA mbox_id and store it in v2h.
 * If timeout_ms is zero, block until a message arrives. Otherwise return after
 * timeout_ms miliseconds if no message is received.
 */
int vNMMMbxReceive(struct nmm_mbox *v2h, uint32_t mbox_id, uint32_t timeout_ms);

#endif /* __NMM_AVI_H__ */
