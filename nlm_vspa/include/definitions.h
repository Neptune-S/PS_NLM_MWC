#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define DMA_TXR_size 					(512)
#define KILO_SIZE 						(1024)
#define AXIQ_UNIT_TXFR_SIZE				(1024)
#define AXIQ_PINGPONG_TXFR_SIZE			(AXIQ_UNIT_TXFR_SIZE)
#define AXIQ_UNIT_TXFR_BYTES			(AXIQ_UNIT_TXFR_SIZE << 2)
#define FFT_SIZE                    	(512)
#define IFFT_SIZE                   	(512)
#define SSB_ITER_PER_FREQ_HYPO      	(1200)  // This for 20ms scanning 
#define N_PSS_FREQ_HYPO			    	(0x5)

#define B_SZ			                (256)
#define CBUF_SZ 		                (2048)  // circular buffer size measured in words (complex samples)
#define SCBUF_SZ                        (2048)

#define SF_PER_FRAME					(10)
#define SAMPLE_PER_BATCH_61_44			(1024)
#define SAMPLE_PER_BATCH_30_72   		(512)
#define SFN_MAX							(1024)
#define SFN_SF_MAX						(10240)

#define SAMP_61_44_PER_SF				(SAMP_PER_BATCH_61_44 * BATCHES_PER_SUBFRAME)
#define SAMP_61_44_PER_FRAME			(SAMP_61_44_PER_SF * SF_PER_FRAME)
#define SAMP_61_44_PER_ALL_SFN			(SAMP_61_44_PER_FRAME * SFN_MAX)

#define PSS_NOISE_RE_HIGH_IDX 			(64)
#define PSS_NOISE_RE_LOW_IDX  			(392)
#define PSS_NOISE_BUFF_OFFSET			(512)

//HOST DDR memory definitions
//#define HOSTS_SCRATCH_MEMORY 			(0xA0419140)
//#define SSDET_REF_OFFSET                (0x0D000)
//#define PSDET_REF_TD_OFFSET             (0x1D000)
//#define PSS_REF_XCORR_OFFSET            (0x2D000)

//FREERTOS DTCM
#define HOSTS_SCRATCH_MEMORY 			(0x20000100)

#define SSDET_REF_SZ             (0x00)
#define PSDET_REF_TD_SZ          (0x1800)	//6K
#define PSS_REF_XCORR_SZ         (0x1800)	//6K
#define OVERLAY_SECTION_SZ		 (0x3C00)  //15K		
#define PBCH_FFT_SYM_SZ          (0x1800)	//6K
#define VSPA_DATA_IN_A_SZ        (0x1400)	//5K
#define VSPA_DATA_IN_B_SZ        (0x700)	//1.75K
#define VSPA_DATA_OUT_A_SZ       (0x700)	//1.75K
#define VSPA_CMD_REG_SZ          (0x200)	//512


#define PSDET_REF_TD_OFFSET             (SSDET_REF_SZ)
#define PSS_REF_XCORR_OFFSET            (PSDET_REF_TD_OFFSET+PSDET_REF_TD_SZ)						
#define PSS_REF_OVL_OFFSET              (PSS_REF_XCORR_OFFSET+PSS_REF_XCORR_SZ)						//
#define PBCH_TOTAL_SYM_OFFSET           (PSS_REF_OVL_OFFSET+OVERLAY_SECTION_SZ)  					//
#define VSPA_DATA_IN_A_OFFSET           (PBCH_TOTAL_SYM_OFFSET + PBCH_FFT_SYM_SZ)					//
#define VSPA_DATA_IN_B_OFFSET           (VSPA_DATA_IN_A_OFFSET + VSPA_DATA_IN_A_SZ)					//
#define VSPA_DATA_OUT_A_OFFSET          (VSPA_DATA_IN_B_OFFSET + VSPA_DATA_IN_B_SZ)					//
#define DTCM_CMD_RSP_ADDRESS_OFFSET     (VSPA_DATA_OUT_A_OFFSET + VSPA_DATA_OUT_A_SZ)				//
#define DTCM_BIT_PROC_OFFSET     		(DTCM_CMD_RSP_ADDRESS_OFFSET + VSPA_CMD_REG_SZ)				//

#define VSPA_DATA_IN_A_ADDR   			(HOSTS_SCRATCH_MEMORY + VSPA_DATA_IN_A_OFFSET )				//
#define VSPA_DATA_IN_B_ADDR  			(HOSTS_SCRATCH_MEMORY + VSPA_DATA_IN_B_OFFSET )				//
#define VSPA_DATA_OUT_A_ADDR   			(HOSTS_SCRATCH_MEMORY + VSPA_DATA_OUT_A_OFFSET )			//
#define DTCM_CMD_RSP_ADDRESS            (HOSTS_SCRATCH_MEMORY + DTCM_CMD_RSP_ADDRESS_OFFSET)		//
#define DTCM_BIT_PROC_ADDRESS           (HOSTS_SCRATCH_MEMORY + DTCM_BIT_PROC_OFFSET)				//



typedef enum {
	p_stop, //0
	p_cs,	//1
	p_mib,	//2
	p_sib,	//3
	p_rssi,	//4
	p_cf,	//5
	p_dec_dump, //6
	p_rx_dump, //7
	p_idle,
	p_cs_cf
} proc_type_t;

typedef struct {
	unsigned int src;
	unsigned int dst;
	unsigned int sz;
} overlay_section_t;

typedef enum {
	ovl_tools, 		//0
	ovl_cell_search,//1
	ovl_cell_search_csrs,//2
	ovl_cell_follow,//3
	ovl_cell_follow_track,//4
	ovl_mib,//5
	ovl_total_num	//6
} overlay_id_t;

#endif /* DEFINITIONS_H_ */
