// =============================================================================
//! @file           system_resources.h
//! @brief          Devices, dma channels, linkers and memory header.
//! @copyright      Copyright 2021 NXP
// =============================================================================
/*  NXP Confidential. This software is owned or controlled by NXP and may only be used strictly
 *  in accordance with the applicable license terms. By expressly accepting
 *  such terms or by downloading, installing, activating and/or otherwise using
 *  the software, you are agreeing that you have read, and that you agree to
 *  comply with and are bound by, such license terms. If you do not agree to
 *  be bound by the applicable license terms, then you may not retain,
 *  install, activate or otherwise use the software.
 */
#ifndef SYSTEM_RESOURCES_H_
#define SYSTEM_RESOURCES_H_


#include <time.h>
#include "ipreg_types.h"
#include "vspa.h"
#include "definitions.h"
#include "la9310.h"
#include "nr_pss.h"
#include "dma.h"

#define V_MAJOR		(0xFEDCBA00)
#define V_MINOR		(5)

#define MBOX_ACK 	(1)
#define MBOX_NACK 	(0)

//DMA ALLOCATION
#define DMA_PROGRAM_CH	(0x0)
#define DMA_RX_RO2_CH 	(0x2)
#define DMA_DDR_WR_CH 	(0x7)
#define DMA_DDR_RD_CH 	(0x9)
#define DMA_TX_CH 		(0xB)
#define DMA_PHY_TIMER	(0xE)
#define DMA_REGS_CH		(0xA)

#define DDR_WR_STEP 				(4*KILO_SIZE)
#define DDR_RD_STEP 				(DMA_TXR_size)
#define SYNC_DONE					(2)	
//AXIQ in use
#define AXIQ_TX_ADDR	 			(0x4400B000)
#define AXIQ_RX_RO2_ADDR 			(0x44002000)
#define DDR_START_ADDRESS           (0xA0219100)
#define DDR_SIZE_BYTE               (0x258000)
#define NO_CMD     					    (0)
#define SSB_DETECTED 					(1)
#define SSB_SCAN_TIMEOUT 				(2)
#define SSB_NOT_DETECTED				(3)
#define PBCH_CAPTURED					(5)
#define CMD_SSBSTOP						(0)
#define CMD_SSBSCAN						(1)	
#define CMD_CELLFOLLOW  				(5)
#define CMD_IQDUMP						(6)
#define CMD_RASTERSCAN					(13)
#define CMD_SSBSCAN_CELLFOLLOW 			(14)
#define CMD_MIB_DECODE		 			(17)

#define CMD_PBCH_DMRS_EXT		 		(19)
#define CMD_PBCH_LS_EST 		 		(20)
#define CMD_PBCH_EQUALISER		 		(21)
#define CMD_MS_START				 	(22)
#define CMD_MS_IND					 	(23)

#define LENGTH_DECIM2X		            (8)

__attribute__(( section(".large_buffer") ))_VSPA_VECTOR_ALIGN
cfixed16_t REF_SEQ_BUF[FFT_SIZE*PSS_NUM_ID2];

__attribute__(( section(".large_buffer") )) 
vspa_complex_fixed16 ippu_buffer[AXIQ_PINGPONG_TXFR_SIZE] _VSPA_VECTOR_ALIGN;

__attribute__(( section(".scratch") ))
cfixed16_t decHist[32*3] _VSPA_VECTOR_ALIGN = {0};	// history for decimators is preserved in separate memory area

__attribute__(( section(".idramvar") )) 
cfixed16_t CBUF_PROC[CBUF_SZ] _VSPA_VECTOR_ALIGN;

__attribute__(( section(".scratch") ))
pss_init_params_t pss_init_params __align_vec;

__attribute__((aligned( 4 * UPHW )))
uint16_t const SSSDET_REF_D0[ 72 ] = {
    #include "../vector/sss_seq_d0_128x3_bits.hex"
};

__attribute__((aligned( 32 * UPHW )))
uint16_t const SSSDET_REF_D1[ 18 ] = {
    #include "../vector/sss_seq_d1_288_bits.hex"
};
extern uint32_t _image_length__first;
extern uint32_t _image_length__second;
extern uint32_t _run_address__first;
extern uint32_t _run_address__second;
extern uint32_t _load_address__first;
extern uint32_t _load_address__second;

__attribute__((aligned(8)))
overlay_section_t ovl_mem[2] = {
		{(uint32_t)&_load_address__first,	(uint32_t)&_run_address__first,		(uint32_t)&_image_length__first},  //0
		{(uint32_t)&_load_address__second,	(uint32_t)&_run_address__second,	(uint32_t)&_image_length__second}, //1
};

__attribute__(( section(".scratchbuff") ))  _VSPA_VECTOR_ALIGN
uint32_t BUF_SCRATCH[SCBUF_SZ];


__attribute__(( section(".scratch") ))
__attribute__((aligned(2 * UPHW))) pssdet_param_T  pssdet_param = {
    .nco5x1 = {-16777216,-8388608,0,8388608,16777216}        
};

__attribute__(( section(".scratch") ))
__attribute__((aligned(2 * UPHW))) sssdet_param_T sssdet_param ;

__attribute__(( section(".scratch") ))
__attribute__((aligned(1 * UPHW))) __fx16 const pssdet_decim2x_coeff[  LENGTH_DECIM2X/2 ] = {0.627078,0.298051,-0.129118,0.017528};//{0.3854,0.18318,-0.079356,0.010773};    

__attribute__((aligned(8 * UPHW))) ssdet_out_T ssdet_out;

// VSPA Controller Memory Mapped Interface. 
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
}vspa_controller_if_t;

extern __attribute__((aligned(128 * UPHW))) vspa_controller_if_t ssb_info;
// DMA
uint32_t channel_rx                = DMA_RX_RO2_CH;
uint32_t load_cmd                  = 0x1;
uint32_t dma_load_addr             = AXIQ_RX_RO2_ADDR;
uint32_t ddr_read_idx              = 0x1;
uint32_t AXIQ_rd_counter           = 0x1;
uint32_t process_iteration         = 0x1;
uint32_t process_iteration_timeout = 0x1;
uint32_t DDR_wr_address, DDR_wr_size;
uint32_t DDR_rd_address, DDR_rd_size;
uint32_t ddrEndAddress;
uint32_t txfr_size;


uint32_t mailbox_status = 0;
uint32_t mboxMSB        = 0;
uint32_t mboxLSB        = 0;
uint32_t procMSB        = 0;
uint32_t procLSB        = 0;
uint32_t goMSB          = 0;
uint32_t goLSB          = 0;
uint32_t curr_read      = 0;

uint32_t* p_PSSDET_REF_PSSSEQTIME = (uint32_t*)(HOSTS_SCRATCH_MEMORY + PSDET_REF_TD_OFFSET);
uint32_t* p_PSSDET_REF_XCORRFFT   = (uint32_t*)(HOSTS_SCRATCH_MEMORY + PSS_REF_XCORR_OFFSET);
uint32_t* p_SSB_PBCH_SYM          = (uint32_t*)(HOSTS_SCRATCH_MEMORY + PBCH_TOTAL_SYM_OFFSET);


/* Decimator Parameters */
cfixed16_t *pDcmOut_s1;
cfixed16_t *pDcmOut_s2;

void decimator(void);

uint32_t read_phytmr(void);

void ssb_scan(uint8_t is_ssbscan_cell_follow);

void ssb_follow(void);

void ssb_follow_mib(void);

uint32_t cell_search(uint32_t iFH);

uint32_t cell_follow(void);

uint32_t cell_follow_mib(void);

void enable_rx2(uint32_t channel_no);

void iq_capture(uint32_t cmd, uint32_t src);

static inline void custom_axiq_disable_rx2(void);

static inline void custom_axiq_enable_rx2(void);

void setup(void);

void change_overlay(uint32_t ovl_id);

#endif /* SYSTEM_RESOURCES_H_ */
