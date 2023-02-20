/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright 2017-2018, 2021-2022 NXP
 * All rights reserved.
 */

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

#define SSB_CMD (((uint32_t)CMD_CELLFOLLOW << 24) & 0xFF000000)
#define MIB_CMD (((uint32_t)CMD_CELL_FOLLOW_MIB << 24) & 0xFF000000)

volatile uint32_t *vspa_controller_info_ddr_add = (volatile uint32_t *)vspa_cmd;
static uint32_t phy_timer_stamp = 0;
static uint32_t crt_freq;
static int32_t last_ppb_adjust = 0;
int16_t llrIn[864];
static int ipc_up = 0;
static uint8_t raster_scan_rsp;
static iir_filter_cfo_param_t iir_cfo_filter_param;
static iir_filter_noise_param_t iir_noise_filter_param;
static TimerHandle_t vspa_timer;
static vspa_controller_if_t vspa_controller_if;
static bool reset_in_progress = false;
static volatile bool dma_done = false;
cell_acquistion_response_t *cell_acquistion_response;
ssb_stats_t *ssb_stats;
mib_info_t *mib_info;

void vDMACallback(void *info, edma_status st)
{
	dma_done = true;
}

static int prvCopyRegion(edma_channel_info_t *pEdmaHandle,
						 uint32_t src, uint32_t dst, uint32_t size)
{
	int ret;

	dma_done = false;
	ret = iEdmaXferReq(src, dst, size, pEdmaHandle);
	if (ret < 0)
	{
		nmm_print("Error in iEdmaXferReq(src = 0x%x, dst = 0x%x, size = %d\r\n",
				  src, dst, size);
		return ret;
	}
	while (!dma_done)
		;

	return 0;
}

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

static int prvCopyVSPAMem(edma_channel_info_t *pEdmaHandle, uint32_t reg_type,
						  uint32_t src_base, uint32_t dst_base)
{
	struct nmm_vspa_mem_ctrl *mem_ctrl;
	struct nmm_vspa_mem_region *reg;
	uint32_t dst_offset = 0, max_offset;
	int i, ret;

	mem_ctrl = (struct nmm_vspa_mem_ctrl *)src_base;
	for (i = 0; i < mem_ctrl->num_regions; i++)
	{
		reg = &mem_ctrl->region[i];
		if (reg->type != reg_type || reg->size == 0)
			continue;

		ret = prvCopyRegion(pEdmaHandle, src_base + reg->offset,
							dst_base + dst_offset, reg->size);
		if (ret < 0)
			return ret;

		nmm_print_dbg("Mem region %s [%dB] at base 0x%x + offset 0x%x in DTCM\r\n",
					  reg->name, reg->size, dst_base, dst_offset);

		switch (reg_type)
		{
		case VSPA_MEM_TBL:
			dst_offset += ALIGN(reg->size, VSPA_MEM_ALIGN);
			max_offset = VSPA_TBL_MEM_SIZE;
			break;
		case VSPA_MEM_OVERLAY:
			if (reg->size > VSPA_OVERLAY_MAX_SIZE)
			{
				nmm_print("Section %s larger than 6.5KB (%dB)\r\n",
						  reg->name, reg->size);
				return -1;
			}
			dst_offset += VSPA_OVERLAY_MAX_SIZE;
			max_offset = VSPA_OVL_MEM_SIZE;
			break;
		default:
			return -1;
		};
		if (dst_offset > max_offset)
		{
			nmm_print("VSPA region %s exceeds reserved space\r\n",
					  reg->name);
			return -1;
		}
	}

	return 0;
}

int prvVSPAMemInit(uint32_t host_mem)
{
	edma_channel_info_t *pEdmaHandle;
	uint32_t ulEdmaInfo = 0xaa55aa55;
	int ret;

	pEdmaHandle = pEdmaAllocChannel(&ulEdmaInfo, (Callback)vDMACallback);

	ret = prvCopyVSPAMem(pEdmaHandle, VSPA_MEM_TBL,
						 host_mem, (uint32_t)vspa_tbl_mem);
	if (ret < 0)
	{
		vEdmaFreeChannel(pEdmaHandle);
		return ret;
	}

	ret = prvCopyVSPAMem(pEdmaHandle, VSPA_MEM_OVERLAY,
						 host_mem, (uint32_t)vspa_ovl_mem);

	if (ret < 0)
	{
		vEdmaFreeChannel(pEdmaHandle);
		return ret;
	}

	vEdmaFreeChannel(pEdmaHandle);

	return 0;
}

static void prvDoMemInit(int dev_id, int qid,
						 struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *msg;

	msg = (struct nmm_msg *)deq_raw_op->in_addr;
	nmm_print_dbg("Received NMM MEM INIT message: addr = 0x%x\r\n", msg->addr);

	prvVSPAMemInit(msg->addr);
}

int vBBDEVSetup(void)
{
	struct dev_attr_t *attr;
	int queue_id, ret;

	/* Wait for bbdev device ready.*/
	while (!bbdev_ipc_is_host_initialized(BBDEV_IPC_DEV_ID_0))
	{
	}

	attr = bbdev_ipc_get_dev_attr(BBDEV_IPC_DEV_ID_0);
	if (!attr)
	{
		nmm_print("Unable to get bbdev attributes\r\n");
		return -1;
	}
	for (queue_id = 0; queue_id < attr->num_queues; queue_id++)
	{
		ret = bbdev_ipc_queue_configure(BBDEV_IPC_DEV_ID_0, queue_id);
		if (ret != IPC_SUCCESS)
		{
			nmm_print("queue configure failed (error %d)\r\n", ret);
			return ret;
		}
	}

	/* We're ready to exchange messages with host */
	ipc_up = 1;

	return 0;
}

static void prvSendRaw(void *buf, uint32_t len)
{
	int dev_id = BBDEV_IPC_DEV_ID_0;
	int qid = BBDEV_IPC_M2H_QUEUE;
	struct bbdev_ipc_raw_op_t enq_raw_op;
	int retries = ENQUEUE_RETRIES;
	int ret = 0;

	if (reset_in_progress)
		return;

	enq_raw_op.in_len = len;
	enq_raw_op.in_addr = (uint32_t)buf;
	enq_raw_op.out_len = 0;
	enq_raw_op.out_addr = 0;

	while (retries--)
	{
		ret = bbdev_ipc_enqueue_raw_op(dev_id, qid, &enq_raw_op);
		if (ret == 0)
			break;
		if (ret == IPC_CH_FULL)
			continue;
	}
	if (ret < 0)
		nmm_print("bbdev_ipc_enqueue_raw_op failed (%d)\r\n", ret);
}

static void prvSendNMMReply(struct nmm_msg *msg)
{
	nmm_print_dbg("Sending message to host:\r\n");
	hexdump((uint8_t *)msg, NMM_MSG_SIZE);

	prvSendRaw((void *)msg, NMM_MSG_SIZE);
}

static void *prvGetTxBuf(int dev_id, int qid)
{
	void *buf;
	uint32_t len;
	int retries = GET_BUF_RETRIES;

	while (retries--)
	{
		buf = bbdev_ipc_get_next_internal_buf(dev_id, qid, &len);
		if (buf)
			return buf;
	}

	nmm_print("Failed to get internal buffer\r\n");
	return NULL;
}

static void prvSendMsgToHost(uint32_t type, uint32_t status)
{
	struct nmm_msg *rsp;

	if (!ipc_up)
		return;

	rsp = (struct nmm_msg *)prvGetTxBuf(BBDEV_IPC_DEV_ID_0,
										BBDEV_IPC_M2H_QUEUE);
	if (!rsp)
		return;

	rsp->type = type;
	rsp->status = status;

	prvSendNMMReply(rsp);
}

/*
 * Send command to VSPA, wait for ACK, message host in case of error
 * or if ACK marks end of VSPA processing
 *
 * @vspa_timeout_ms speficies the maximum amount of time we wait for
 * VSPA to finish processing the command and notify us an output is ready.
 * If vspa_timeout is > 0, further processing by VSPA can take at most
 * <vspa_timeout> ms.
 * If == 0, no further processing is needed.
 * if < 0, further processing is needed but no timeout is imposed
 */
static void prvSendVspaCmd(struct nmm_mbox *mbox_h2v, uint32_t rsp_type,
						   int32_t vspa_timeout_ms)
{
	struct nmm_mbox mbox_v2h = {0};
	uint32_t status = NMM_STATUS_ERR;
	int ret;

	ret = vNMMMbxSend(mbox_h2v, NMM_OPS_MBOX_ID);
	/*
	 * If send succeeded, wait for an ACK from VSPA; otherwise just
	 * signal the error back to host
	 */
	if (ret != 0)
	{
		nmm_print("Error sending message to VSPA\r\n");
		goto send_ack;
	}

	ret = vNMMMbxReceive(&mbox_v2h, NMM_OPS_MBOX_ID, VSPA_ACK_TIMEOUT_MS);

	/*
	 * Start timer right after receiving VSPA ACK, to avoid the corner case
	 * where "processing done" message is received from VSPA before timer
	 * is actually started.
	 */
	if (vspa_timeout_ms > 0)
		prvStartVspaTimer(rsp_type, vspa_timeout_ms);

	nmm_print_dbg("VSPA reply: ret = %d, ACK = %d, err = %d\r\n",
				  ret, mbox_v2h.status.ack, mbox_v2h.status.err_code);
	if (ret != 0)
	{
		status = NMM_STATUS_TIMEOUT;
		goto send_ack;
	}
	if (mbox_v2h.status.ack && mbox_v2h.status.err_code == 0)
	{
		/* If VSPA has more processing to do, we're done here */
		if (vspa_timeout_ms != VSPA_IMM_RESPONSE)
			return;
		/* Otherwise notify host of success */
		status = NMM_STATUS_OK;
	}
	else if (vspa_timeout_ms > 0)
	{
		/* If VSPA returns an error, don't wait for a "done" message */
		xTimerStop(vspa_timer, 0);
	}

send_ack:
	prvSendMsgToHost(rsp_type, status);
}

static void prvDoRxCapture(int dev_id, int qid,
						   struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	int ret;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	nmm_print_dbg("Received Rx capture request: addr = 0x%x, len %d\r\n",
				  req->addr, req->size);

	mbox_h2v.ctrl.opcode = NMM_DBG_RX_CAPTURE;
	mbox_h2v.ctrl.param1 = NUM_BLOCKS(req->size);
	mbox_h2v.param2 = req->addr;

	ret = prvSetRadio(req);
	if (ret)
	{
		prvSendMsgToHost(NMM_RX_CAPTURE, NMM_STATUS_ERR);
		return;
	}

	/* VSPA comm */
	prvSendVspaCmd(&mbox_h2v, NMM_RX_CAPTURE, VSPA_RX_CAPTURE_TIMEOUT_MS);
}

static void prvDoFillMemCustom(int dev_id, int qid,
						   struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	uint32_t offset_addr;
	
	offset_addr = FILL_MEM_DDR_WR_ADDRESS;      
	req = (struct nmm_msg *)deq_raw_op->in_addr;

    prvSendMsgToHost(NMM_FILL_MEM_CUSTOM, NMM_STATUS_OK);

	log_info("\nReceived fill_mem_custom request: addr = 0x%x, len %d, off:%p\r\n",
				  req->addr, req->size,offset_addr);
	
	memcpy((uint32_t *)offset_addr, (uint32_t *)req->addr,req->size);

	log_info("\nDDR filled with custom data\n");

}

static void prvDoBandScanN77(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	prvSendMsgToHost(NMM_BAND_SCAN_N77, NMM_STATUS_OK);
	sendCliCommandToVspa(CMD_RASTERSCAN);
}

static void prvDoSsbScan(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	prvSendMsgToHost(NMM_SSB_SCAN, NMM_STATUS_OK);
	sendCliCommandToVspa(CMD_SSBSCAN);
}

static void prvDoDecCapture(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	int ret;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	nmm_print_dbg("Received Rx capture request: addr = 0x%x, len %d\r\n",
				  req->addr, req->size);

	mbox_h2v.ctrl.opcode = NMM_DBG_DEC_CAPTURE;
	mbox_h2v.ctrl.param1 = NUM_BLOCKS(req->size);
	mbox_h2v.param2 = req->addr;

	ret = prvSetRadio(req);
	if (ret)
	{
		prvSendMsgToHost(NMM_DEC_CAPTURE, NMM_STATUS_ERR);
		return;
	}

	/* VSPA comm */
	prvSendVspaCmd(&mbox_h2v, NMM_DEC_CAPTURE, VSPA_DEC_CAPTURE_TIMEOUT_MS);
}

void parse_vspa_cell_info(vspa_controller_if_t *vspa_controller_if)
{
	uint8_t *pIsValid = NULL;
	pIsValid = (uint8_t *)(vspa_controller_info_ddr_add + 10); //DDR valid flag

	if (*pIsValid == 1) //ensure all ddr data is written
	{
		*pIsValid = 0; //reset

		vspa_controller_if->cellId = *(vspa_controller_info_ddr_add);
		vspa_controller_if->rssi = *(vspa_controller_info_ddr_add + 1) / NUM_SUBCARRIER_SSS;
		vspa_controller_if->cfo = *(vspa_controller_info_ddr_add + 2);
		vspa_controller_if->process_itration = *(vspa_controller_info_ddr_add + 3);
		vspa_controller_if->corr_real = *(vspa_controller_info_ddr_add + 4);
		vspa_controller_if->corr_imag = *(vspa_controller_info_ddr_add + 5);
		vspa_controller_if->noise_pwr = *(vspa_controller_info_ddr_add + 6) / NUM_SUBCARRIER_NOISE;
		vspa_controller_if->t0 = *(vspa_controller_info_ddr_add + 7);
		vspa_controller_if->t_delta_minor = *(vspa_controller_info_ddr_add + 8);
		vspa_controller_if->t_delta_major = *(vspa_controller_info_ddr_add + 9);
	}
	else
		memset(vspa_controller_if, 0, sizeof(vspa_controller_if_t));
}

static void prvVspaDone(struct nmm_mbox *v2h, enum nmm_msg_type type)
{
	int32_t nco_freq;
	vspa_controller_if_t vspa_controller_if;

	if (xTimerIsTimerActive(vspa_timer))
		xTimerStop(vspa_timer, 0);

	if (type == NMM_CELL_SEARCH)
	{
		if ((v2h->msb & 0x1) == 1)
		{
			parse_vspa_cell_info(&vspa_controller_if);

			cell_acquistion_response->cell_search_cell_id = vspa_controller_if.cellId;
			cell_acquistion_response->cell_rssi           = (float)vspa_controller_if.rssi;
			cell_acquistion_response->cell_noise          = (float)vspa_controller_if.noise_pwr;
			
			/* SNR */
            float snr = (cell_acquistion_response->cell_rssi-cell_acquistion_response->cell_noise)/(cell_acquistion_response->cell_noise);

			if(snr > 0)
			   cell_acquistion_response->cell_snr_dB = 10*log2(snr)*.30102;
			else
			   cell_acquistion_response->cell_snr_dB = -100;

			convert_to_db(cell_acquistion_response->cell_rssi ,&cell_acquistion_response->cell_rssi_dBm);
			convert_to_db(cell_acquistion_response->cell_noise, &cell_acquistion_response->cell_noise_dBm);
					
			nco_freq = (int32_t)((roundf((float)vspa_controller_if.cfo * FS_BY_TWO_POWER_32)) / 1000); // 0.00357 =  (fs)/(2^32)

			log_info("CellId    : %d \n\r", cell_acquistion_response->cell_search_cell_id);
			log_info("SNR_x10   : %d dB\n\r", (int32_t)(10*cell_acquistion_response->cell_snr_dB));
			log_info("CFO       : %d KHz\n\r", nco_freq);
			log_info("Status    : FOUND \n\r");
		}
		else
		{
			cell_acquistion_response->cell_search_cell_id = INVALID_CELLID;
			log_info("CellId    : NA \n\r");			
			log_info("SNR_x10   : NA dB \n\r");
			log_info("Cfo       : NA KHz\n\r");
			log_info("Status    : TIME_OUT \n\r");
		}
	}
}

void prvDoRasterScan(struct nmm_mbox *v2h)
{
	static uint16_t gscnidx = 0; 

	if (xTimerIsTimerActive(vspa_timer))
		xTimerStop(vspa_timer, 0);

	parse_vspa_cell_info(&vspa_controller_if);

	log_info("GSCN  : %d \n\r", (N77_BAND_GSCN_START + gscnidx));
	cell_acquistion_response->gscn  = (N77_BAND_GSCN_START + gscnidx);
	
	gscnidx++;
	
	if(gscnidx == MAX_GSCN) //NUM of gscns in  n77 band
	gscnidx = 0;

	if ((v2h->msb & 0x1) == 1)
	{
		cell_acquistion_response->cell_search_cell_id = vspa_controller_if.cellId;
		
		
		cell_acquistion_response->cell_rssi  = (float)vspa_controller_if.rssi;
		cell_acquistion_response->cell_noise = (float)vspa_controller_if.noise_pwr;
		
		/* SNR */
        float snr = (cell_acquistion_response->cell_rssi-cell_acquistion_response->cell_noise)/(cell_acquistion_response->cell_noise);

		if(snr > 0)
			cell_acquistion_response->cell_snr_dB = 10*log2(snr)*.30102;
		else
			cell_acquistion_response->cell_snr_dB = -100;

		convert_to_db(cell_acquistion_response->cell_rssi, &cell_acquistion_response->cell_rssi_dBm);
		convert_to_db(cell_acquistion_response->cell_noise, &cell_acquistion_response->cell_noise_dBm);
		
		cell_acquistion_response->cell_rssi_dBm -= gain_correction_gscn_db[cell_acquistion_response->gscn-N77_BAND_GSCN_START] - 30 + RSSI_OFFSET_DB;
		raster_scan_detected_ssb++;
		raster_scan_rsp = 1;

		struct nmm_msg *cpy_rsp;

	    if(!ipc_up)
		return;
	    cpy_rsp = (struct nmm_msg *)prvGetTxBuf(BBDEV_IPC_DEV_ID_0,
										BBDEV_IPC_M2H_QUEUE);
	    if(!cpy_rsp){
		return;}
		memcpy(&cpy_rsp->cell_acquistion_response,cell_acquistion_response,sizeof(cell_acquistion_response_t));
		cpy_rsp->type = NMM_GET_BAND_STATS;
	    prvSendNMMReply(cpy_rsp);

		log_info("CellId    : %d \n\r", cell_acquistion_response->cell_search_cell_id);
		log_info("RSSI_x10  : %d dBm \n\r", (int32_t)(10*cell_acquistion_response->cell_rssi_dBm));
		log_info("SNR_x10   : %d dB \n\r", (int32_t)(10*cell_acquistion_response->cell_snr_dB));
		//memcpy structure 
		
	}
	else
		log_info("Status: SSB NOT FOUND \n\n\r");
	
}

uint8_t prvSearchMaxCorr(float* dmrsSsbCorrVal)
{
	uint8_t idx;
	float maxVal = dmrsSsbCorrVal[0];
	uint8_t maxIdx = 0;

	for(idx = 1; idx  < MAX_NUM_SSB_BLOCKS; idx++)
	{
		if(dmrsSsbCorrVal[idx] > maxVal)
		{
			maxVal = dmrsSsbCorrVal[idx];
			maxIdx = idx;	
		}
	}

	log_dbg("prvSearchMaxCorr: Max Idx: %d\n\r", (int32_t)(maxIdx*1000));
	return maxIdx;
}

void prvDoPbchChannelEstimation(struct nmm_mbox* v2h)
{
	if ((v2h->msb & 0x1) == 1)
		xSemaphoreGive(xSemVSPABusy);
}

void prvDoPbchChannelEqualization(struct nmm_mbox* v2h)
{
	if ((v2h->msb & 0x1) == 1)
		xSemaphoreGive(xSemVSPABusy);	
	
}

void prvDoPbchSearchSSBIndex(struct nmm_mbox* v2h)
{
	if ((v2h->msb & 0x1) == 1)
	{
		//dmrsSsbCorrVal[dmrsSsbIdx] = v2h->lsb;

		xSemaphoreGive(xSemVSPABusy);
	}
}

void prvDoCellFollowMIB(struct nmm_mbox *v2h)
{
	xSemaphoreGive(xSemVSPABusy);

	if ((v2h->msb & 0x1) == 1)
	{ 
		parse_vspa_cell_info(&vspa_controller_if);
		xSemaphoreGive(xSemMIBTaskUnlock);
	}
	else
	{
		ssb_stats->n_missed_detection += 1;
	}
}

static void prvEnableIntTest(int dev_id, int qid, struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	nmm_print_dbg("Received Test mode enable command: addr = 0x%x, len %d\r\n",
				  req->addr, req->size);

	mbox_h2v.ctrl.opcode = NMM_DBG_INT_TEST_ENABLE;
	mbox_h2v.ctrl.param1 = NUM_BLOCKS(req->size);
	mbox_h2v.param2 = req->addr;

	prvSendVspaCmd(&mbox_h2v, NMM_INT_TEST_ENABLE, VSPA_IMM_RESPONSE);
}

static void prvDisableIntTest(int dev_id, int qid,
							  struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_mbox mbox_h2v = {0};

	nmm_print_dbg("Received Test mode disable cmd\r\n");

	mbox_h2v.ctrl.opcode = NMM_DBG_INT_TEST_DISABLE;
	prvSendVspaCmd(&mbox_h2v, NMM_INT_TEST_DISABLE, VSPA_IMM_RESPONSE);
}

static void prvDoCellSearch(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	int ret;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	mbox_h2v.cmd.opcode = NMM_OP_CELL_SEARCH;
	mbox_h2v.out_addr = req->addr;

	nmm_print_dbg("Received cell search request: addr = 0x%x\r\n", req->addr);

	ret = prvSetRadio(req);
	if (ret)
	{
		prvSendMsgToHost(NMM_CELL_SEARCH, NMM_STATUS_ERR);
		return;
	}

	prvSendVspaCmd(&mbox_h2v, NMM_CELL_SEARCH, VSPA_CELL_SEARCH_TIMEOUT_MS);
}

static void prvDoCellFollow(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;

	req = (struct nmm_msg *)deq_raw_op->in_addr;
	cell_acquistion_response->gscn = req->gscn;

	prvSendMsgToHost(NMM_CELL_FOLLOW, NMM_STATUS_OK);
	sendCliCommandToVspa(CMD_CELLFOLLOW);
}

static void prvDoStop(int dev_id, int qid,
					  struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_mbox mbox_h2v = {0};

	nmm_print_dbg("Received NMM STOP cmd\r\n");

	mbox_h2v.ctrl.opcode = NMM_OP_NMM_STOP;
	prvSendVspaCmd(&mbox_h2v, NMM_STOP, VSPA_IMM_RESPONSE);

	if (xTimerIsTimerActive(vspa_timer))
		xTimerStop(vspa_timer, 0);
}

static void prvDoCellFollowStop(int dev_id, int qid,
					  struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	prvSendMsgToHost(NMM_CELL_FOLLOW_STOP, NMM_STATUS_OK);
	sendCliCommandToVspa(CMD_SSBSCAN_CELLFOLLOWSTOP);
}


static void prvDoMIB(int dev_id, int qid, struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	cell_acquistion_response->gscn = req->gscn;

	prvSendMsgToHost(NMM_CELL_FOLLOW_MIB, NMM_STATUS_OK);
	sendCliCommandToVspa(CMD_CELL_FOLLOW_MIB);
}

static void prvDoSIB(int dev_id, int qid, struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	int ret;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	mbox_h2v.cmd.opcode = NMM_OP_SIB_PROCEDURE;
	mbox_h2v.cmd.cell_id = req->cell_id;
	mbox_h2v.cmd.interval = req->update_int;
	mbox_h2v.out_addr = req->addr;

	nmm_print_dbg("Received system info. request: addr = 0x%x\r\n", req->addr);

	ret = prvSetRadio(req);
	if (ret)
	{
		prvSendMsgToHost(NMM_DO_SIB, false);
		return;
	}

	prvSendVspaCmd(&mbox_h2v, NMM_DO_SIB, VSPA_DUMP_TIMEOUT_MS);
}

static void prvDoTimeUpdate(int dev_id, int qid,
							struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	struct nmm_mbox mbox_h2v = {0};
	uint32_t tshift, sfn;
	uint32_t phy_timer_read;

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	nmm_print_dbg("Received time update, sfn: %d\r\n", req->sfn);

	mbox_h2v.ctrl.opcode = NMM_OP_TIME_UPDATE;
	mbox_h2v.param2 = req->sfn & 0x0000FFFF;

	prvSendVspaCmd(&mbox_h2v, req->type, VSPA_UPDATE_TIMEOUT_MS);

	tshift = (req->sfn >> 16) & 0x0000FFFF;
	sfn = mbox_h2v.param2;
	sfn = sfn % 100;		 //Keep the time that is less than 1 second. Every 100 sfn is 1 sec
	sfn = (100 - sfn) * 10;	 //Time goes forward in unit of ms
	tshift = tshift + sfn;	 // sum the shifts and sfn time @ 1ms
	tshift = tshift * 61440; // convert to 61.44MHz ticks

	phy_timer_read = ulPhyTimerCapture(PHY_TIMER_COMP_PPS_OUT);
	if (phy_timer_read < phy_timer_stamp)
	{
		// take care of wraps in the phy timer, skip 1 second
		phy_timer_stamp += PLAT_FREQ;
	}
	if (phy_timer_read > (uint32_t)(phy_timer_stamp + tshift))
	{
		// too much time has passed, skip 1 second
		phy_timer_stamp += PLAT_FREQ;
	}

	vPhyTimerPPSOUTAdjustMajor(phy_timer_stamp, tshift);

	nmm_print_dbg("phy_timer_stamp: %d, time_shift: %d, both in tick@61.44MHz\r\n",
				  phy_timer_stamp, tshift);
}

static void prvDoDLDump(int dev_id, int qid,
						struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_msg *req;
	struct nmm_mbox mbox_h2v = {0};

	req = (struct nmm_msg *)deq_raw_op->in_addr;

	nmm_print_dbg("Received DL dump request\n");

	/* For MIB, T (SI periodicity) = 0 */
	mbox_h2v.ctrl.opcode = req->si_T ? NMM_OP_SI_DUMP : NMM_OP_MIB_DUMP;
	mbox_h2v.ctrl.param1 = req->si_T / 10; /* VSPA expects # of frames */
	mbox_h2v.param2 = (req->si_n << 16) | req->si_w;

	prvSendVspaCmd(&mbox_h2v, req->type, VSPA_UPDATE_TIMEOUT_MS);
}

static void prvDoReset(int dev_id, int qid,
					   struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	nmm_print_dbg("Received a reset request\r\n");

	/* Trigger a reset: Arm the timer and then loop forever */
	prvStartVspaTimer(NMM_RESET, 1);
	while (true)
		;
}

static void prvDoIPCReset(int dev_id, int qid,
						  struct bbdev_ipc_raw_op_t *deq_raw_op)
{
	struct nmm_mbox mbox_h2v = {0};

	nmm_print_dbg("Received an IPC reset notification from host\r\n");

	/*
	 * No response needed, host has already cut off IPC Rx path.
	 * Just do local cleanup and prepare for new IPC configuration
	 */

	/* Suspend Rx loops */
	ipc_up = 0;

	/* Stop VSPA from any current processing */
	mbox_h2v.ctrl.opcode = NMM_OP_NMM_STOP;
	prvSendVspaCmd(&mbox_h2v, NMM_IPC_RESET, NO_TIMEOUT);

	/* and clear timer if needed */
	if (xTimerIsTimerActive(vspa_timer))
		xTimerStop(vspa_timer, 0);

	/* Wait for all in-flight traffic to finish processing */
	vTaskDelay(1000);

	/* stop and restart IPC */
	bbdev_ipc_close(0, 0);
	if (bbdev_ipc_init(0, 0))
	{
		nmm_print("BBDEV IPC init failed\r\n");
		return;
	}
	if (vBBDEVSetup())
	{
		nmm_print("BBDEV IPC setup failed\n");
		return;
	}

	bbdev_ipc_signal_ready(dev_id);
}

static RxProcessFn xFnArray[NMM_MAX_OPS] = {
	[NMM_RX_CAPTURE] = prvDoRxCapture,
	[NMM_DEC_CAPTURE] = prvDoDecCapture,
	[NMM_INT_TEST_ENABLE] = prvEnableIntTest,
	[NMM_INT_TEST_DISABLE] = prvDisableIntTest,
	[NMM_CELL_SEARCH] = prvDoCellSearch,
	[NMM_CELL_FOLLOW] = prvDoCellFollow,
	[NMM_CELL_FOLLOW_MIB] = prvDoMIB,
	[NMM_CELL_FOLLOW_STOP] = prvDoCellFollowStop,
	[NMM_DO_SIB] = prvDoSIB,
	[NMM_STOP] = prvDoStop,
	[NMM_TIME_UPDATE] = prvDoTimeUpdate,
	[NMM_DL_DUMP] = prvDoDLDump,
	[NMM_VSPA_MEM_INIT] = prvDoMemInit,
	[NMM_FILL_MEM_CUSTOM] = prvDoFillMemCustom,
	[NMM_SSB_SCAN] = prvDoSsbScan,
	[NMM_BAND_SCAN_N77] = prvDoBandScanN77,
	[NMM_RESET] = prvDoReset,
	[NMM_IPC_RESET] = prvDoIPCReset,
};

static RxProcessFn prvGetRxFunc(uint32_t type)
{
	if (type >= NMM_MAX_OPS)
		return NULL;

	return xFnArray[type];
}

static void prvProcessHostRx(int dev_id, int qid)
{
	struct bbdev_ipc_raw_op_t *deq = NULL;
	struct nmm_msg *msg;
	RxProcessFn fn;
	int is_ipc_reset;
	int ret;
	RficResp_t ret1;
	static uint8_t configRficOnce = 1;

	if (configRficOnce == 1)
	{
		
		configRficOnce = 0;
		vTaskDelay(1000);
		ret1 = vConfigRfic(0);
		if (ret1)
			log_info("Config RFIC return  %d \n\r", ret1);
		else
			log_info("Config RFIC success \n\r");
	}

	do
	{
		if (!ipc_up)
			continue;
		deq = bbdev_ipc_dequeue_raw_op(dev_id, qid);
	} while (!deq);

	nmm_print_dbg("Received message from host: deq = %p, in = 0x%x, out = 0x%x, len = %d\r\n",
				  deq, deq->in_addr, deq->out_addr, deq->in_len);
	hexdump((uint8_t *)deq->in_addr, deq->in_len);

	msg = (struct nmm_msg *)deq->in_addr;
	is_ipc_reset = (msg->type == NMM_IPC_RESET);

	fn = prvGetRxFunc(msg->type);
	if (fn)
		fn(dev_id, qid, deq);
	else
		nmm_print("Received unknown message type (%d)\r\n", msg->type);

	/* If BBDEV IPC was reset, don't consume stale deq op */
	if (is_ipc_reset)
		return;

	ret = bbdev_ipc_consume_raw_op(dev_id, qid, deq);
	if (ret)
		nmm_print("bbdev_ipc_consume_raw_op failed(%d)\n\r", ret);
}

void vRxLoop(void *pvParameters)
{
	int dev_id = BBDEV_IPC_DEV_ID_0;
	int qid = BBDEV_IPC_H2M_QUEUE;

	/* Signal Host to start sending packets for processing */
	bbdev_ipc_signal_ready(dev_id);
	nmm_print("Waiting for messages from host...\n\r");

	while (1)
	{
		prvProcessHostRx(dev_id, qid);
	}
}

void prvCellFollowAdjust(struct nmm_mbox *v2h)
{
	int32_t ppb_x10;
	int32_t phy_tmr_t_off;
	int32_t f_off;

	// VSPA calculates it's relative offset
	phy_tmr_t_off = ((int32_t)(v2h->ctrl.param1 << 8) >> 8);
	if (phy_tmr_t_off)
	{
		vPhyTimerPPSOUTAdjustMinor(phy_tmr_t_off);
	}
	nmm_print("\tPHY Timer adjusted by [%d ticks]\r\n", phy_tmr_t_off);

	f_off = (int)v2h->param2;
	if (f_off > MAX_FREQ_THRES)
	{
		f_off = MAX_FREQ_THRES;
	}
	else if (f_off < -MAX_FREQ_THRES)
	{
		f_off = -MAX_FREQ_THRES;
	}

	// PPB = param2 Hz / carrier_freq kHz * 10^6 = Hz/Hz*10^9     ppb_x10 = PPB*10
	ppb_x10 = (int32_t)roundf(((float)f_off) * 10 * 1000000 / crt_freq);
	last_ppb_adjust = last_ppb_adjust + ppb_x10;
	nmm_print("\tFrequency offseted by %d Hz, adjusting [%d PPB*10]\r\n", (int)v2h->param2, last_ppb_adjust);

	if (f_off)
	{
		prvSetVCXO(last_ppb_adjust);
		/* Redo radio calibration */
		prvCalRadio();
	}
}

static void prvTimerCb(TimerHandle_t timer)
{
	uint32_t rsp_type;

	rsp_type = (uint32_t)pvTimerGetTimerID(timer);
	prvSendMsgToHost(rsp_type, NMM_STATUS_TIMEOUT);
	reset_in_progress = true;
}

static char *prvOpcodeToStr(uint8_t opcode)
{
	switch (opcode)
	{
	case NMM_CTRL_CELL_SEARCH:
		return "Cell search";
	case NMM_CTRL_RX_CAPTURE:
		return "Rx capture";
	case NMM_CTRL_DEC_CAPTURE:
		return "Decimated Rx capture";
	case NMM_CTRL_CELL_FOLLOW:
		return "Cell follow";
	case NMM_CTRL_MIB_PROCEDURE:
		return "MIB procedure";
	case NMM_CTRL_SIB_PROCEDURE:
		return "SIB procedure";
	default:
		return "N/A";
	}
}

void prvDoCellFollowCellSearch(struct nmm_mbox *v2h)
{
	int32_t ffo = 0;
	vspa_controller_if_t vspa_controller_if;

	if (xTimerIsTimerActive(vspa_timer))
		xTimerStop(vspa_timer, 0);

	parse_vspa_cell_info(&vspa_controller_if);
	xSemaphoreGive(xSemVSPABusy);

	if ((v2h->msb & 0x1) == 1)
	{

		/* Calibrate VCXO from the CFO at the cell search stage */ 	
		ffo = (int32_t)(roundf((float)vspa_controller_if.cfo * FS_BY_TWO_POWER_32)); // 0.003576278 = (fs)/(2^32)
		last_ppb_adjust = (int32_t)roundf(((float)ffo) * 10 * 1000000 / getCrtFreq());
       
		prvSetVCXO(last_ppb_adjust);
		/* Redo radio calibration */
		prvCalRadio();
        
		
		cell_acquistion_response->cell_follow_cell_id = vspa_controller_if.cellId;
		
		/* Measurements */
		cell_acquistion_response->cell_rssi  = (float)vspa_controller_if.rssi;
		cell_acquistion_response->cell_noise = (float)vspa_controller_if.noise_pwr;
		
		convert_to_db(cell_acquistion_response->cell_rssi, &cell_acquistion_response->cell_rssi_dBm);
		convert_to_db(cell_acquistion_response->cell_noise, &cell_acquistion_response->cell_noise_dBm);
		
		/* Calculate SNR */
        float snr = ((cell_acquistion_response->cell_rssi-cell_acquistion_response->cell_noise))/(cell_acquistion_response->cell_noise);
        
		if(snr>0)
			cell_acquistion_response->cell_snr_dB = 10*log2(snr)*0.30102; //log10 = log2.*30102;
		else
            cell_acquistion_response->cell_snr_dB = -100;
        
		/* Calibrate RSSI */
		cell_acquistion_response->cell_rssi_dBm -= gain_correction_gscn_db[cell_acquistion_response->gscn - N77_BAND_GSCN_START] - 30 + RSSI_OFFSET_DB;
        cell_acquistion_response->cell_tracking = 1; 
		/* Clear Filters */
		memset(&iir_cfo_filter_param,0,sizeof(iir_filter_cfo_param_t)); 
		memset(&iir_noise_filter_param,0,sizeof(iir_filter_noise_param_t)); 

		vPhyTimerPPSOUTConfigGPSlike();

#ifdef PRINT_CONSOLE
		log_info("CellId      : %d \n\r", cell_acquistion_response->cell_follow_cell_id);
		log_info("RSSI_x10    : %d dBm \n\r", (int32_t)(10 * cell_acquistion_response->cell_rssi_dBm));
		log_info("SNR_x10     : %d dB\n\r", (int32_t)(10 * cell_acquistion_response->cell_snr_dB));
		log_info("CFO         : %d KHz\n\r", ffo / 1000);
		log_info("ppbx_10     : %d \n\r", last_ppb_adjust);
		log_info("Status	  : CELL_FOUND \n\r");
		
#endif
	}
	else
	{
#ifdef PRINT_CONSOLE
		log_info("NO_CELL_FOUND\n\r");
#endif
	}
}

void prvDoTrackingUpdate(struct nmm_mbox *v2h, enum nmm_msg_type type)
{

	iir_filter_cfo_param_t *cfo_filter         = &iir_cfo_filter_param;;
	iir_filter_noise_param_t *noise_filter     = &iir_noise_filter_param;
	vspa_controller_if_t vspa_controller_if;
	
	static uint8_t one_sec_cnt = 0;
    int32_t ppb_x10            = 0;

	xSemaphoreGive(xSemVSPABusy);

	parse_vspa_cell_info(&vspa_controller_if);
	schedular_params->process_itr = ((v2h->msb >> 1) & 0xFFFF);

	if ((v2h->msb & 0x1) != 1)
	{
		schedular_params->cons_msd_ssb_det++;
		ssb_stats->n_missed_detection += 1;
	}
	else
	{
		if (schedular_params->t_delta_minor_last != 0)
		{   
			schedular_params->t_delta_minor_latest = vspa_controller_if.t_delta_minor;
			schedular_params->t_delta_minor = (schedular_params->t_delta_minor_last - schedular_params->t_delta_minor_latest);            
		}
		else
			schedular_params->t_delta_minor_last = vspa_controller_if.t_delta_minor;
		
		taskENTER_CRITICAL();
		schedular_params->t0_update -= schedular_params->t_delta_minor;	
		taskEXIT_CRITICAL();
		
		schedular_params->pps_adjust = -schedular_params->t_delta_minor;
		vPhyTimerPPSOUTAdjustMinor(schedular_params->pps_adjust);

		if (vspa_controller_if.cellId == cell_acquistion_response->cell_follow_cell_id)
		{
			schedular_params->cons_msd_ssb_det = 0;
			
			cfo_filter->r_real = ((1 - ALPHA_CFO) * cfo_filter->r_real) + (ALPHA_CFO * (vspa_controller_if.corr_real));	
			cfo_filter->r_imag = ((1 - ALPHA_CFO) * cfo_filter->r_imag) + (ALPHA_CFO * (vspa_controller_if.corr_imag));
			
			cfo_filter->angle_est = atan2(cfo_filter->r_imag, cfo_filter->r_real);
		    cfo_filter->cfo_est   = (cfo_filter->angle_est * BASEBAND_SAMPLING_RATE) / (2 * M_PI * 2 * SYMBOL_SIZE);

			ppb_x10 = (int32_t)roundf(((float)cfo_filter->cfo_est) * 10 * 1000000 / getCrtFreq());
			last_ppb_adjust = last_ppb_adjust + ppb_x10;
            
			/* Update Noise Filter */
	           
			noise_filter->r = (((1 - ALPHA_NOISE) * noise_filter->r) + (ALPHA_NOISE * vspa_controller_if.noise_pwr));
		
			if ((schedular_params->schedule_mib == MIB_ENABLE)&&((-(PPB_3GPP_LIM*10) < ppb_x10) && (ppb_x10 < (PPB_3GPP_LIM * 10))))
				schedular_params->schedule_mib = MIB_SCHEDULE;
			else	
			    schedular_params->schedule_mib &= ~(1 << 1);
		    
			/* Calibrate Radio */
			prvSetVCXO(last_ppb_adjust);
			/* Redo radio calibration */
			prvCalRadio();
		    
			/* Measurments */
			cell_acquistion_response->cell_rssi  = (float)vspa_controller_if.rssi;
            cell_acquistion_response->cell_noise = (float)noise_filter->r;

			/* SNR */
			float snr = ((cell_acquistion_response->cell_rssi - cell_acquistion_response->cell_noise)/(cell_acquistion_response->cell_noise));

			if(snr>0) 
				cell_acquistion_response->cell_snr_dB =  10*log2(snr)*0.30102;
            else
			    cell_acquistion_response->cell_snr_dB  = -100;

			convert_to_db(cell_acquistion_response->cell_rssi, &cell_acquistion_response->cell_rssi_dBm);
			convert_to_db(cell_acquistion_response->cell_noise, &cell_acquistion_response->cell_noise_dBm);
			
			cell_acquistion_response->cell_rssi_dBm -= gain_correction_gscn_db[cell_acquistion_response->gscn - N77_BAND_GSCN_START] - 30 + RSSI_OFFSET_DB;
	
			#ifdef PRINT_CONSOLE
			if(one_sec_cnt == 250)
			{
				one_sec_cnt = 0;

				log_info("[SSB_TRACK_UPDATE]: SFN/SF: %d/%d, CellId: %d, RSSI_x10: %d dBm, SNR_x10: %d dB, cfo_x1000:%d Hz, ppb_x10: %d\n\r",
					schedular_params->gnb_sfn,
				 	schedular_params->gnb_sf,
				 	cell_acquistion_response->cell_follow_cell_id,
				 	(int32_t)(10 * cell_acquistion_response->cell_rssi_dBm),
				 	(int32_t)(10 * cell_acquistion_response->cell_snr_dB),
					(int32_t)(cfo_filter->cfo_est * 1000),
					(int32_t)(ppb_x10));
			
		    	log_info("[SSB_STATS_UPDATE]: n_tx_ssb: %d, pmd_x100000: %d, pfa_x100000: %d, n_tx_mib: %d, n_mib_bler_x100000: %d\n\n\r",
					 ssb_stats->n_rx_ssb,
				 	((ssb_stats->n_missed_detection + ssb_stats->n_false_alarm) * 100000) / ssb_stats->n_rx_ssb,
				 	(ssb_stats->n_false_alarm * 100000) / ssb_stats->n_rx_ssb,
				 	ssb_stats->n_rx_mib,
				 	(ssb_stats->n_mib_crc_fail * 100000) / (ssb_stats->n_rx_mib + 1));
				struct nmm_msg *cpy_rsp;

	            if (!ipc_up)
		        return;
	            cpy_rsp = (struct nmm_msg *)prvGetTxBuf(BBDEV_IPC_DEV_ID_0,
										BBDEV_IPC_M2H_QUEUE);
	            if (!cpy_rsp)
		        return;
			    memcpy(&cpy_rsp->schedular_params,schedular_params,sizeof(schedular_params_t));
			    memcpy(&cpy_rsp->cell_acquistion_response,cell_acquistion_response,sizeof(cell_acquistion_response_t));
			    memcpy(&cpy_rsp->ssb_stats,ssb_stats,sizeof(ssb_stats_t));
				memcpy(&cpy_rsp->iir_filter_cfo_param,cfo_filter,sizeof(iir_filter_cfo_param_t));
	            cpy_rsp->type = NMM_GET_CELL_STATS_INFO;
	            prvSendNMMReply(cpy_rsp);
			
			 
			}else
			one_sec_cnt++;
#endif	
		}else
		  ssb_stats->n_false_alarm += 1;
	}	
	
	if (schedular_params->cons_msd_ssb_det >= 50)
	{   
		/* Stop Timers */
		schedular_params->cons_msd_ssb_det = 0;
		vPhyTimerComparatorDisable(PHY_TIMER_COMP_VSPA_GO_0);
		vPhyTimerComparatorDisable(PHY_TIMER_COMP_PPS_OUT);
		schedular_params->schedule_mib = MIB_NOT_SCHEDULE;
		
		/* Clear Structures */
		memset(ssb_stats, 0, sizeof(ssb_stats_t));
		memset(schedular_params, 0, sizeof(schedular_params_t));
		memset(cell_acquistion_response, 0, sizeof(cell_acquistion_response_t));
		
		/* Update VCXO */
		prvSetVCXO(0);
		/* Redo radio calibration */
		prvCalRadio();

		#ifdef PRINT_CONSOLE
			log_info("CELL FOLLOW SYNC LOST !!!\n\r");
		#endif
	}

	if (xTimerIsTimerActive(vspa_timer))
		xTimerReset(vspa_timer, 0);

}

void vSchedularLoop(void *pvParameters)
{
	struct nmm_mbox vspa_ctrl_msg;
	struct avi_mbox host_mbox;
	uint32_t ret = 0;
	uint16_t tti_cnt;
	host_mbox.lsb = 0;

	while (1)
	{
		/* Check for message */
		ret = vNMMMbxReceive(&vspa_ctrl_msg, NMM_OPS_MBOX_ID, 0);
		if (ret != 0)
			continue;

		switch (vspa_ctrl_msg.ctrl.opcode)
		{
		case NMM_CTRL_MS_START:
			schedular_params->t0_update = vspa_ctrl_msg.lsb;
			schedular_params->t0_update = ((schedular_params->t0_update + 61440) & 0xFFFFFFFF);
			vPhyTimerVspaGoComp0(schedular_params->t0_update);
			break;
		case NMM_CTRL_MS_IND:
			vPhyTimerComparatorForce(PHY_TIMER_COMP_VSPA_GO_0, ePhyTimerComparatorOut0);
			schedular_params->t0_update = ((schedular_params->t0_update + 61440) & 0xFFFFFFFF);
			vPhyTimerVspaGoComp0(schedular_params->t0_update);

			schedular_params->ue_sf += 1;
			schedular_params->gnb_sf += 1;
			if (schedular_params->ue_sf == 10)
			{
				schedular_params->ue_sf = 0;
				schedular_params->ue_sfn += 1;
				schedular_params->ue_sfn &= 0x3FF;
			}

			if (schedular_params->gnb_sf == 10)
			{
				schedular_params->gnb_sf = 0;
				schedular_params->gnb_sfn += 1;
				schedular_params->gnb_sfn &= 0x3FF;
			}

			tti_cnt = (schedular_params->ue_sfn * 10) + schedular_params->ue_sf;

			if(schedular_params->ue_sf == 9)
			{
				if(((schedular_params->ue_sfn & 0xF) == 0xF) && (schedular_params->schedule_mib == MIB_SCHEDULE))
				{
					host_mbox.msb = MIB_CMD;
					sendMboxtoVSPA(host_mbox);
					ssb_stats->n_rx_mib += 1;
					ssb_stats->n_rx_ssb += 1;
					schedular_params->mib_scheduled_tti_cnt = tti_cnt;
					schedular_params->mib_scheduled_sfn = schedular_params->ue_sfn;
				}
				else if(schedular_params->ue_sfn & 0x1)
				{
					host_mbox.msb = SSB_CMD; /*Opcode */
					sendMboxtoVSPA(host_mbox);
					ssb_stats->n_rx_ssb += 1;
				}
			}
			
			if((schedular_params->gnb_sfn == 0) && (schedular_params->gnb_sf == 0) && (!schedular_params->pps_timer_start))
			{
				uint64_t  update_cnt = ulPhyTimerComparatorRead(PHY_TIMER_COMP_PPS_OUT);
				update_cnt += ((schedular_params->t_delta_minor_latest + MSECONDS_TO_PHY_TIMER_COUNT(1000)) & 0xFFFFFFFF);
				vPhyTimerUpdateComparator(PHY_TIMER_COMP_PPS_OUT, (uint32_t)update_cnt);
				schedular_params->pps_timer_start = 1;
			}

			schedular_params->tti_cnt = tti_cnt;

			break;

		default:
			break;
		}
	}
}

void vVSPALoop(void *pvParameters)
{

	struct nmm_mbox vspa_ctrl_msg;
	uint32_t ret = 0;

	vspa_timer = xTimerCreate("vspa_timer", 1, pdFALSE, 0, prvTimerCb);
	if (vspa_timer == NULL)
		nmm_print("Error creating VSPA timer\r\n");

    /* Clearing structures. Can be moved to host task in the future. */
    memset(cell_acquistion_response,0,sizeof(cell_acquistion_response_t));
    memset(ssb_stats,0,sizeof(ssb_stats_t));

	while (1)
	{
		/* Check for message */
		ret = vNMMMbxReceive(&vspa_ctrl_msg, NMM_CTRL_MBOX_ID, 0);
		if (ret != 0)
			continue;

		nmm_print_dbg("Receive msg from VSPA: msb = %x,lsb = %x opcode = %s\r\n",
					  vspa_ctrl_msg.msb, vspa_ctrl_msg.lsb,
					  prvOpcodeToStr(vspa_ctrl_msg.ctrl.opcode));

		switch (vspa_ctrl_msg.ctrl.opcode)
		{
		case NMM_OP_NMM_STOP:

			break;
		case NMM_CTRL_CELL_SEARCH:
			prvVspaDone(&vspa_ctrl_msg, NMM_CELL_SEARCH);
			break;
		case NMM_CTRL_RX_CAPTURE:
			prvVspaDone(&vspa_ctrl_msg, NMM_RX_CAPTURE);
			break;
		case NMM_CTRL_DEC_CAPTURE:
			prvVspaDone(&vspa_ctrl_msg, NMM_DEC_CAPTURE);
			break;
		case NMM_CTRL_CELL_FOLLOW:
			prvDoTrackingUpdate(&vspa_ctrl_msg, NMM_CELL_FOLLOW);
			break;
		case NMM_CTRL_MIB_PROCEDURE:
			prvDoTrackingUpdate(&vspa_ctrl_msg, NMM_DO_MIB);
			break;
		case NMM_CTRL_SIB_PROCEDURE:
			prvDoTrackingUpdate(&vspa_ctrl_msg, NMM_DO_SIB);
			break;
		case NMM_CTRL_RASTER_SCAN:
			prvDoRasterScan(&vspa_ctrl_msg);
			break;
		case NMM_CTRL_CELL_FOLLOW_CELL_SEARCH:
			prvDoCellFollowCellSearch(&vspa_ctrl_msg);
			break;
		case NMM_CTRL_CELL_FOLLOW_MIB:
			prvDoCellFollowMIB(&vspa_ctrl_msg);
			break;
		case NMM_CTRL_PBCH_SEARCH_SSB_INDEX:
			prvDoPbchSearchSSBIndex(&vspa_ctrl_msg);
			break;
		case NMM_CTRL_PBCH_CHANNEL_ESTIMATOR:
			prvDoPbchChannelEstimation(&vspa_ctrl_msg);
			break;	
		case NMM_CTRL_PBCH_CHANNEL_EQUALIZER:
			prvDoPbchChannelEqualization(&vspa_ctrl_msg);
			break;

		default:
			nmm_print("Received undefined opcode from VSPA: 0x%x\r\n",
					  vspa_ctrl_msg.ctrl.opcode);
			break;
		}
	}
}

uint8_t prvGetRasterScanRsp()
{
	return raster_scan_rsp;
}

void ps_nr_pbch_channel_interpolation()
{

	long var_ls_est_real =0;
	long var_ls_est_imag =0;
	int scaling_factor	 =0;
	int num_bits		 =0;
	int ls_est_idx		 =0;
	long var = 0;
	cplx_fxp_int est_per_rb[3] ;
	
	cplx_fxp_int* ls_est = (cplx_fxp_int*)vspa_data_out_a_mem;
	cplx_fxp_int* channel_est = (cplx_fxp_int*)vspa_data_in_a_mem;
	
	for (int i = 0; i < PBCH_DMRS_RE_COUNT; i++)
	{
		var_ls_est_real = var_ls_est_real + (ls_est[i].real)* (ls_est[i].real);
		var_ls_est_imag = var_ls_est_imag + (ls_est[i].imag) * (ls_est[i].imag);
	}
	var = (var_ls_est_real + var_ls_est_imag) / PBCH_DMRS_RE_COUNT;

	num_bits = (14 - ceil(log2(sqrt(var))));  
	scaling_factor = (num_bits > 0) ? num_bits:0;

	for (int i = 0; i < DATA_RE_PER_PBCH; i = i + DATA_RE_PER_RB)
	{
		est_per_rb[0].real = ls_est[ls_est_idx].real << scaling_factor;
		est_per_rb[0].imag = ls_est[ls_est_idx].imag << scaling_factor;
		est_per_rb[1].real = ls_est[ls_est_idx+1].real << scaling_factor;
		est_per_rb[1].imag = ls_est[ls_est_idx + 1].imag << scaling_factor;
		est_per_rb[2].real = ls_est[ls_est_idx+2].real << scaling_factor;
		est_per_rb[2].imag = ls_est[ls_est_idx + 2].imag << scaling_factor;

		channel_est[i]     = est_per_rb[0];
		channel_est[i + 1] = est_per_rb[0];
		channel_est[i + 2] = est_per_rb[0];
		channel_est[i + 3] = est_per_rb[1];
		channel_est[i + 4] = est_per_rb[1];
		channel_est[i + 5] = est_per_rb[1];
		channel_est[i + 6] = est_per_rb[2];
		channel_est[i + 7] = est_per_rb[2];
		channel_est[i + 8] = est_per_rb[2];
		ls_est_idx         = ls_est_idx + 3;
	}

}

void ps_nr_pbch_demod()
{
	int16_t* llr_in = (int16_t*)&llrIn[0];
	cplx_fxp_int* equalized_data = (cplx_fxp_int*)vspa_data_out_a_mem;
	int llr_idx = 0;
	for (int j = 0; j < DATA_RE_PER_PBCH; j++)
	{
		llr_in[llr_idx] = equalized_data[j].real;
		llr_in[llr_idx + 1] = equalized_data[j].imag;
		llr_idx = llr_idx + 2;
	}
}

void ps_nr_pbch_update_sf(uint8_t ssbIdx, uint8_t hf, uint8_t *sf, uint8_t *sym)
{
	switch (ssbIdx)
	{
	case 0:
		*sym = 2;
		*sf = 0;
		break;
	case 1:
		*sym = 8;
		*sf = 0;
		break;
	case 2:
		*sym = 16;
		*sf = 0;
		break;
	case 3:
		*sym = 22;
		*sf = 0;
		break;
	case 4:
		*sym = 30;
		*sf = 1;
		break;
	case 5:
		*sym = 36;
		*sf = 1;
		break;
	case 6:
		*sym = 44;
		*sf = 1;
		break;
	case 7:
		*sym = 50;
		*sf = 1;
		break;
	default:
		break;
	}

	if (hf)
		*sf += 5;
}

void prvResetRasterScanRsp()
{
	raster_scan_rsp = 0;
}

void vMibLoop(void *pvParameters)
{
	static uint8_t ssbDetectedIdx = INVALID_SSB_IDX;
	uint32_t pnX1[2] = { 0,0 };
	uint32_t pnX2[2] = { 0,0 };
	uint32_t cwCrc   = 0;
	uint32_t calCrc  = 0;
	uint32_t bitIn   = 0;
	uint32_t bitOut  = 0;
	uint16_t seqLen  = 864;
	uint16_t cellId;
	uint8_t numSsbBlocks = 8;
	uint8_t payloadLen   = 32;
	uint16_t noise       = 0;
	uint8_t cmd;
	uint16_t sfn_incr;
	uint8_t sf_incr, mib_sf, mib_sym;

 	while(1)
	{
		xSemaphoreTake(xSemMIBTaskUnlock, (TickType_t)portMAX_DELAY);

		if (vspa_controller_if.cellId == cell_acquistion_response->cell_follow_cell_id)
			schedular_params->cons_msd_ssb_det = 0;
		else
			ssb_stats->n_false_alarm += 1;

		cellId = cell_acquistion_response->cell_follow_cell_id;

		ps_nr_pbch_re_demapper((int16_t*)ssb_pbch_sym_mem, (cplx_fxp_int*) vspa_data_in_b_mem, (cplx_fxp_int*)scratchBuf, cellId);
		
		xSemaphoreTake(xSemVSPABusy, (TickType_t)portMAX_DELAY );
			
		ps_nr_pbch_dmrs_gen((cplx_fxp_int*)&vspa_data_in_a_mem[0],cellId);
			
		cmd = CMD_PBCH_SEARCH_SSB_INDEX;
		host_mbox.lsb = 0;
		host_mbox.msb = 0;													
		host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
		sendMboxtoVSPA(host_mbox);
			
		xSemaphoreTake(xSemVSPABusy, (TickType_t)portMAX_DELAY);

		ssbDetectedIdx = INVALID_SSB_IDX;
		ssbDetectedIdx = prvSearchMaxCorr((float*)&vspa_data_out_a_mem[0]);
		
		cplx_fxp_int* src_drms_addr = (cplx_fxp_int*)&vspa_data_in_a_mem[0] + ssbDetectedIdx * 144;
		cplx_fxp_int* dest_drms_addr = (cplx_fxp_int*)&vspa_data_in_a_mem[0];
		for (int i = 0; i < 144; i++)
		{
			dest_drms_addr[i] = src_drms_addr[i];
		}
		
		cmd = CMD_PBCH_CHANNEL_ESTIMATOR;
		host_mbox.lsb = 0;
		host_mbox.msb = 0;													 
		host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
		sendMboxtoVSPA(host_mbox);
			
		xSemaphoreTake(xSemVSPABusy, (TickType_t)portMAX_DELAY);
		xSemaphoreGive(xSemVSPABusy);
		
		ps_nr_pbch_channel_interpolation();
		
		xSemaphoreTake(xSemVSPABusy, (TickType_t)portMAX_DELAY);
		
		cplx_fxp_int* data_re_scrbuff = (cplx_fxp_int*)scratchBuf;
		cplx_fxp_int* data_re_in_b    = (cplx_fxp_int*)vspa_data_in_b_mem;

		for (int j = 0; j < DATA_RE_PER_PBCH; j++)
		{
			data_re_in_b[j].real = data_re_scrbuff[j].real;
			data_re_in_b[j].imag = data_re_scrbuff[j].imag; 
		}
		
		cmd = CMD_PBCH_CHANNEL_EQUALIZER;
		
		noise = (cell_acquistion_response->cell_noise*33);//2^15/1000;

		host_mbox.lsb = noise;
		host_mbox.msb = 0;													  //(24-bit lsb of msb, in units of 1536 Samples)
		host_mbox.msb = host_mbox.msb | (((uint32_t)cmd << 24) & 0xFF000000); /*Opcode */
		
		sendMboxtoVSPA(host_mbox);
		
		xSemaphoreTake(xSemVSPABusy, (TickType_t)portMAX_DELAY);
		xSemaphoreGive(xSemVSPABusy);
		ps_nr_pbch_demod(); 
        
		uint16_t* pbchDeRateMatchIndices = (uint16_t*)&scratchBuf_ping[0];
		ps_nr_pbch_deratematch_indices(pbchDeRateMatchIndices);

		ps_nr_pseudo_random_generator_init(cellId, pnX1, pnX2);

		uint8_t* scrCw = (uint8_t*)&scratchBuf_pong[0];
		
		seqLen = 864;
		
		ps_nr_pseudo_random_generator(seqLen, scrCw, ssbDetectedIdx, pnX1, pnX2);

		ps_nr_pbch_sym_descrambler(llrIn, llrIn, scrCw);

		float* llrOut = (float*)&scratchBuf_pong[0];

		ps_nr_pbch_deratematching(&llrIn[0], &llrOut[0], pbchDeRateMatchIndices);

		ps_nr_pbch_polar_decoder(&llrOut[0], &bitIn, &cwCrc, scratchBuf);

		calCrc = ps_nr_pbch_crc(&bitIn, payloadLen);

		if ((calCrc == cwCrc) && (cwCrc != 0))
		{
			seqLen = 116; // 4 * M;

			ps_nr_pseudo_random_generator(seqLen, scrCw, 0, pnX1, pnX2);

			ps_nr_pbch_bit_descrambling(&bitIn, numSsbBlocks, scrCw);

			bitOut = ps_nr_pbch_bit_deinterleaver(bitIn);

			ps_nr_mib_parsing(bitOut, mib_info);

			ps_nr_pbch_update_sf(ssbDetectedIdx, mib_info->half_frame_bit, &mib_sf, &mib_sym);

			schedular_params->gnb_sf = mib_sf;
			schedular_params->gnb_sym = mib_sym;
			schedular_params->mib_crc_flag = 1;

			taskENTER_CRITICAL();
			if (schedular_params->tti_cnt < schedular_params->mib_scheduled_tti_cnt)
			{
				schedular_params->mib_proc_delay = (10239 + schedular_params->tti_cnt - schedular_params->mib_scheduled_tti_cnt);
			}
			else
			{
				schedular_params->mib_proc_delay = (schedular_params->tti_cnt - schedular_params->mib_scheduled_tti_cnt);
			}

			sfn_incr = schedular_params->mib_proc_delay / 10;
			sf_incr = schedular_params->mib_proc_delay - (sfn_incr * 10);

			schedular_params->gnb_sfn = (mib_info->sfn + sfn_incr);
			schedular_params->gnb_sf += sf_incr;

			if (schedular_params->gnb_sf > 9)
			{
				schedular_params->gnb_sfn++;
				schedular_params->gnb_sf %= 10;
			}
			schedular_params->gnb_sfn &= 0x3FF;
			taskEXIT_CRITICAL();
			schedular_params->schedule_mib = MIB_NOT_SCHEDULE;

		    struct nmm_msg *nmm_msg;
		
	        if (!ipc_up)
	        return;
		
        
	        nmm_msg = (struct nmm_msg *)prvGetTxBuf(BBDEV_IPC_DEV_ID_0,
									BBDEV_IPC_M2H_QUEUE);
	        if(!nmm_msg)
		    {
		    	log_info("\nbuffer failed");
		    }
	        memcpy(&nmm_msg->mib_info,mib_info,sizeof(mib_info_t));

		    nmm_msg->type = NMM_CELL_FOLLOW_MIB_INFO;
			prvSendNMMReply(nmm_msg);
			
		}
		else
		{
			schedular_params->mib_crc_flag = 0;
			ssb_stats->n_mib_crc_fail += 1;
		}
	}
}
