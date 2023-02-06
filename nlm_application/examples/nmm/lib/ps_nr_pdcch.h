
#include <sys/types.h>
#include "armral.h"
#include <string.h>


#ifndef __PS_NR_PDCCH_H
#define __PS_NR_PDCCH_H

#define MAX_PDCCH_AGGREGATION               (16)
#define NCCE                                (6)
#define MAX_PDCCH_AGGREGATION_LEVELS        (3)
#define PDCCH_DATA_SC_PER_RB                (9)
#define PDCCH_DMRS_SC_PER_RB                (3)
#define N_RE_PER_RB                         (12)
#define N_DMRS_RE_PDCCH                     (3)
#define MAX_INT_16                          (1<<15)

typedef struct {
    uint32_t fdra;
    uint32_t tdra;
    bool mapping;
    uint32_t mcs;
    uint32_t rv;
    bool si_ind;
}dci_1_0_si_sib_1_t;

void ps_nr_pdcch_coreset_extraction(armral_cmplx_int16_t *inSym,uint8_t nSym,uint8_t symb_offset,uint8_t rb_offset,uint8_t rbs,uint8_t kssb,armral_cmplx_int16_t *outSym, void * scratchBuf);
void ps_nr_pdcch_channel_estimator(armral_cmplx_int16_t *dmrs_re,armral_cmplx_int16_t *dmrs_ref_re,armral_cmplx_int16_t *chan_est_re, void* scratchBuf,int16_t n_re);
void ps_nr_pdcch_equalizer(armral_cmplx_int16_t *in_data_re,armral_cmplx_int16_t *chan_est_re,armral_cmplx_int16_t *eq_data_re, int16_t n_re);
void ps_nr_pseudo_random_generator_init(uint32_t cInit, uint32_t* pnX1, uint32_t* pnX2);
void ps_nr_pseudo_random_generator(uint32_t seqLen, uint8_t* c, uint8_t ssbIndex, uint32_t* pnX1, uint32_t* pnX2);
void ps_nr_pdcch_deratematching(int16_t* llr, float* llrOut, uint16_t N, uint16_t E, uint16_t K, void* scratchBuf);
void ps_nr_pdcch_sym_descrambler(int16_t* llrDeScr, int16_t* llr, uint32_t seqLen, uint8_t* pScrCw);
void ps_nr_qpsk_demodulation(armral_cmplx_int16_t* eqDataRe, uint32_t seqLen, int16_t* llr);
void ps_nr_pdcch_gen_rb_interleaving_indices(uint8_t numCoresetRBs, uint8_t numCoresetSym, uint16_t cellId, uint8_t* cceRegInterleaving);
void ps_nr_pdcch_gen_rb_indices(uint8_t* cceRegInterleaverIndices, uint8_t aggrLevel, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t* cceCandidates, uint8_t* rbCandPerAgg);
void ps_nr_pdcch_gen_cce_candidates(uint8_t aggLvl, uint8_t maxNumCand, uint8_t numCCEs, uint8_t candIdx, uint8_t* cceCandidates);
void sort(uint8_t *inputIndices, uint8_t len);
void swap(uint8_t* a, uint8_t* b);
void ps_nr_pdcch_reg_to_re_demapping(uint8_t* regCandPerAgg,  uint8_t aggrLevel, armral_cmplx_int16_t* out1, armral_cmplx_int16_t* out2, armral_cmplx_int16_t* in);
void ps_nr_pdcch_dmrs_gen(armral_cmplx_int16_t* dmrsRefOut, uint16_t cellId, uint16_t slotnum, uint8_t dmrsStartSymNum, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t aggrLvl, uint8_t* rbCandPerAgg, void * scratch_buf);
void ps_nr_parse_dci_1_0_si_sib_1( uint32_t* payload, uint32_t payloadlen, dci_1_0_si_sib_1_t* dci_1_0_si_sib_1);
bool ps_nr_pdcch_crc_check( uint32_t* bitsIn, uint32_t rnti, uint32_t K);
void ps_nr_pdcch_polar_decoder(float *llr, uint32_t *infoBits, uint32_t *info_bit_pattern, uint8_t *interleaver_pattern, void *scratchBuf, uint16_t N, uint8_t K); //int16_t* llrOut
void bitSum(uint8_t *bits, uint8_t *bits_1, uint16_t node, uint16_t temp);
void g_repetition(float *L_depth, float *L_depth_1, uint8_t *bits, uint16_t node, uint16_t temp);
void f_min_sum(float *L_depth, float *L_depth_1, uint16_t node, uint16_t temp);
void ps_nr_pdcch_calculate_K_N_E( uint8_t aggrLvl, uint8_t coreset_rbs, uint8_t* K, uint16_t* E, uint16_t* N);
void ps_nr_polar_information_bit_pattern(uint8_t K, uint16_t E, uint16_t N, uint32_t* info_bit_pattern, void* pScrCw1, void* pScrCw2, void* pScrCw3); 
void ps_nr_polar_bit_interleaver_pattern(uint8_t K, uint16_t I_il, uint8_t* interleaver_pattern);
uint32_t reverseBits(uint32_t num);

#endif /* __NMM_PDCCH_H */