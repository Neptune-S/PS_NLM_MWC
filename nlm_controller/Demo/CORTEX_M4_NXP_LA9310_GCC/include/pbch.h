#ifndef PBCH_H_
#define PBCH_H_
#include "semphr.h"

#define VSPA_TBL_MEM_SIZE       (12 * 1024)
#define VSPA_OVL_MEM_SIZE       (15 * 1024)
#define SSB_PBCH_SYM_MEM_SIZE   (6 * 1024)
#define VSPA_DATA_IN_A 	     	(5 * 1024)
#define VSPA_DATA_IN_B 		    (1792)
#define VSPA_DATA_OUT_A		    (1792)
#define VSPA_CMD			    (512)

#define MAX_NUM_SSB_BLOCKS  (8)
#define PBCH_DMRS_RE_COUNT  (144)

#define DATA_RE_PER_RB  	(9)
#define DATA_RE_PER_PBCH  	(432)

extern SemaphoreHandle_t xSemMIBTaskUnlock;
extern SemaphoreHandle_t xSemVSPABusy;


typedef struct
{
    int16_t real;
    int16_t imag;
}cplx_fxp_int;

uint16_t scratchBuf_ping[1024] __attribute__((section(".vspa_dmem_scr_ping")));
uint16_t scratchBuf_pong[1024] __attribute__((section(".vspa_dmem_scr_pong")));
uint8_t  scratchBuf[6144*2] __attribute__((section(".vspa_dmem_scr_buff")));
uint8_t vspa_tbl_mem[VSPA_TBL_MEM_SIZE] __attribute__((section(".vspa_tbl")));
uint8_t vspa_ovl_mem[VSPA_OVL_MEM_SIZE] __attribute__((section(".vspa_ovl")));
int16_t ssb_pbch_sym_mem[SSB_PBCH_SYM_MEM_SIZE/2] __attribute__((section(".vspa_controller_fft_mem")));
uint8_t vspa_data_in_a_mem[VSPA_DATA_IN_A] __attribute__((section(".shared_mem_a")));
uint8_t vspa_data_in_b_mem[VSPA_DATA_IN_B] __attribute__((section(".shared_mem_b")));
uint8_t vspa_data_out_a_mem[VSPA_DATA_OUT_A] __attribute__((section(".shared_mem_out_a")));
uint8_t vspa_cmd[VSPA_CMD] __attribute__((section(".shared_mem_cmd")));

void ps_nr_pseudo_random_generator_init(uint32_t cInit, uint32_t *pnX1,uint32_t *pnX2);
void ps_nr_pseudo_random_generator(uint32_t seqLen ,uint8_t* c ,uint8_t ssbIndex, uint32_t *pnX1, uint32_t *pnX2);
void ps_nr_pbch_sym_descrambler(int16_t* llrDeScr, int16_t* llrIn,  uint8_t* pScrCw);
void ps_nr_pbch_deratematching(int16_t* llrIn, float* llrOut, uint16_t * pbchDeRateMatchIndices);
void ps_nr_pbch_polar_decoder(float *llr, uint32_t *infoBits, uint32_t *crc,  uint8_t* scratchBuf);
uint32_t ps_nr_pbch_crc(uint32_t *inBits, uint32_t payloadLen);
void f_min_sum(float *L_depth, float *L_depth_1, uint16_t node, uint16_t temp);
void g_repetition(float *L_depth, float *L_depth_1, uint8_t *bits, uint16_t node, uint16_t temp);
void bitSum(uint8_t *bits, uint8_t *bits_1, uint16_t node, uint16_t temp);
void ps_nr_pbch_bit_descrambling(uint32_t* bitIn, uint8_t numSsbBlocks, uint8_t* pScrCw);
void ps_nr_pbch_deratematch_indices(uint16_t *pbchDeRateMatchIndices);
uint32_t ps_nr_pbch_bit_deinterleaver(uint32_t bitIn);
void ps_nr_pbch_re_demapper(int16_t* fftData, cplx_fxp_int* dmrsRe, cplx_fxp_int* dataRe, uint16_t cellId);
void ps_nr_pbch_dmrs_gen(cplx_fxp_int* dmrsRefOut, uint16_t cellId);
void ps_nr_pbch_channel_interpolation();
void ps_nr_pbch_demod();
void ps_nr_pbch_update_sf(uint8_t ssbIdx, uint8_t hf, uint8_t* sf, uint8_t* sym);

#endif
