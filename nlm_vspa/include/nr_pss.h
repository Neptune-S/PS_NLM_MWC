// =============================================================================
//! @file       nr_pss.h
//! @brief      NR PSS kernels and data types.

#ifndef __nr_pss__
#define __nr_pss__
#include <vspa/intrinsics.h>

#include "vspa.h" 
#include "commonTypes.h"
#include "vspa2_fw.h"

#define IPREG_IPPU_CTRL 0x1C0
#define IPREG_IPPU_STAT 0x1C1

#define SIZEVEC8          (_AU_COUNT * 8)        					// Number of bytes per vector.
#define SIZEVEC16         (_AU_COUNT * 4)        					// Number of half-words per vector.
#define SIZEVEC32         (_AU_COUNT * 2)        					// Number of words per vector.
#define SIZEVEC64         (_AU_COUNT * 1)        					// Number of double-words per vector.


#define __align_vec       __attribute__((aligned(SIZEVEC16)))           // Align to vector-boundary.
#define __align_vspa      __attribute__((aligned(AXI_WIDTH_VSPA / 16))) // Align to VSPA AXI-boundary.
#define __align_feca      __attribute__((aligned(AXI_WIDTH_FECA / 16))) // Align to FECA AXI-boundary.
#define UPHW              1                         					// Minimum addressable units per half-word.

#define PSS_NUM_ID2     (3)
#define PSS_THRE		(0.095)
#define SSS_THRE		(0.0293)

//! @brief      PSS detection parameters
typedef struct {
   int32_t nco5x1[5];         //!< negative of nco freqs of 5 freq hypothesis
} pssdet_param_T;

//! @brief      SSS detection parameters
typedef struct {
    float thre_sss;             //!< threshold for sss detection
    float scaling;        //!< scaling to compensate 1/8-scaling from ld.qam and 127-scaling from rssi_sss = 127 * [(1/8)^4] = 0.0310058593750
} sssdet_param_T;

typedef struct {

    // Below: PSS stuff
    float peak_val_pss;             //!< peak normalized xcorr_squared ("xcn" from pss)
    cfixed16_t* ptrPssData_1536;    //!< ptr to 1st sample of PSS data portion @15.36msps =
                                    //!< pss15[9] of total pss15[137] symbol
    uint32_t id2;                   //!< physical cell id: ID2 = {0,1,2}
    int32_t nco_freqhypo;               //!< NCO value of CFO frequency hypothesis from cold PSS search
    cfloat32_t atan2_pss_in;        //!< input to PSS atan2
    cfixed16_t* ptrPssData_3072;    //!< ptr to <Ofst30>-th sample of PSS[0:273] symbol @30.72msps.
                                    //! "Ofst30" macro is defined in "pssdet_xcorr_fft_asm.sx":
                                    //! At OFST30=8, this points to pss30[8], i.e. middle of 18-pt CP.
                                    //! At OFST30=16, this points to pss30[16], i.e. first sample of data part
    int32_t nco_psscfo;             //!< NCO value of PSS-based coarse CFO estimate @30.72 MSPS.
    float rssi_pss;                 //!< Moving sum of squares of most recent 128 samples.
    //float rcp_power;
    // SSS stuff
    float rssi_sss;                 //!< Sum of squares of equalized SSS REs (127x1 vector)
    float peak_val_sss;             //!< value of the peak (from sss)
    uint32_t id1;                   //!< physical cell id: ID1 = {0,1,...,335}
    uint32_t cellid;                //!< physical cell id: ID1*336+ID2
    cfloat32_t atan2_sss_in;        //!< input to SSS atan2
    int nco_ssscfo;                 //!< NCO value = coarse PSS + fine PSS/SSS CFO estimates
    bool freq_lock;
	float track_ffo_corr_re;
	float track_ffo_corr_im;
	float noise_est;
} ssdet_out_T;

//! @brief      SSB detection output struct: In the order of occurrence
/*
typedef struct {

    float peak_val_pss;             //!< peak normalized xcorr_squared ("xcn" from pss)
    cfixed16_t* ptrPssData_1536;    //!< ptr to 1st sample of PSS data portion @15.36msps =
    uint32_t id2;                   //!< physical cell id: ID2 = {0,1,2}
    int32_t nco_freqhypo;               //!< NCO value of CFO frequency hypothesis from cold PSS search
    int32_t nco_psscfo;             //!< NCO value of PSS-based coarse CFO estimate @30.72 MSPS.
    float rssi_pss;                 //!< Moving sum of squares of most recent 128 samples.
    float rssi_sss;                 //!< Sum of squares of equalized SSS REs (127x1 vector)
    float peak_val_sss;             //!< value of the peak (from sss)
    uint32_t id1;                   //!< physical cell id: ID1 = {0,1,...,335}
    uint32_t cellid;                //!< physical cell id: ID1*336+ID2
    int nco_ssscfo;                 //!< NCO value = coarse PSS + fine PSS/SSS CFO estimates
	float track_ffo_corr_re;
	float track_ffo_corr_im;
} ssdet_out_T;
*/
//! @brief      PSS initialization parameters.
typedef struct {
    cfixed16_t  *px30_cbuf_beg;       //!< 30.72 MSPS circular buffer base address.
    size_t      px30_cbuf_size;       //!< 30.72 MSPS circular buffer size.
    uint32_t    *pscr_buf;            //!< pointer to scratch buffer.
    uint32_t    isInitBuffer;         //!< flag for buffer initialized.
    ssdet_out_T *pOutStruct;          //!< SS detected parameters
    float       pssThresh;            //!< PSS thresholds, L1C/L1SP-Int can change
} pss_init_params_t;


//extern unsigned pssdet_decim2x_coeff[] __attribute__((aligned(1 * UPHW)));
extern __attribute__((aligned(2)))  sssdet_param_T sssdet_param;

extern __attribute__((aligned(8)))  ssdet_out_T ssdet_out;

__attribute__((aligned(2))) extern pssdet_param_T  pssdet_param;

extern pss_init_params_t pss_init_params __align_vec;

extern void fftBR512HP();
extern void extract_sss();
extern bool bitRev512Invoke(unsigned int *BUF_SCRATCH,unsigned int *px_bitrev_out);
extern void bitRev512( void );
extern void copy_vec_asm(float* input_address,float* output_address, uint32_t L);
extern uint32_t compensate_cfo_cplx(
		cfixed16_t*  pOut,
		cfixed16_t*  pIn,
		cfixed16_t*  pInBuffBase,
		size_t 		 pInBuffSize, 
		int		     nco_freq, 
		uint32_t	 nco_phase_in, 
        size_t       lines
		);

extern uint32_t nr_sss_xcorr_full_asm(
		cfixed16_t*  pIn,
		cfixed16_t*  pScratch,
		uint32_t 	 id2
		);

extern void	vmult_conj_hfx_hfx(cfixed16_t* x1,cfixed16_t* x2,cfixed16_t* y,cfixed16_t* px_cbuff, uint32_t cbuff_size,uint32_t L);
extern void vmult_conj_vecalgn_hfl_hfl(cfloat16_t* x1,cfloat16_t* x2,cfloat16_t* y, size_t L);
//extern void vmult_conj_vecalgn_hfx_hfx(cfixed16_t* x1,cfixed16_t* x2,cfixed16_t* y, size_t L);
extern void vmult_correlation_vecalgn_asm(cfixed16_t* x1,cfloat16_t* x2,float *corr_re, float *corr_im,size_t L);
extern void clr_buff_asm( cfixed16_t* buff, size_t L);
extern void ifftshift_buff_format(fixed16_t* sss_fd_seq, cfixed16_t* start_addr,cfixed16_t* sub_end_high  );
extern float vsum_vecalgn_hfx(cfixed16_t* inp,uint32_t L, float *sum_re, float *sum_im);
extern void  ssb_idx_search(cfixed16_t* x1,cfixed16_t* x2,float* y);
extern void mmse_equalizer_vecalgn(cfixed16_t* data_re_in,cfixed16_t* ch_est_in,cfixed16_t* equ_re_out, uint16_t N, size_t L);

uint32_t nr_pss_cell_search(
								cfixed16_t *px, 
								cfixed16_t *px_cbuf_beg,
								uint32_t *pscr_buf,
								size_t px_cbuf_size,
								uint32_t iFH
							);

uint32_t nr_pss_cell_follow(
								cfixed16_t *px, 
								cfixed16_t *px_cbuf_beg,
								uint32_t *pscr_buf,
								size_t px_cbuf_size,
								uint32_t isInitBuffer
							);

uint32_t nr_pss_cell_follow_mib(
        cfixed16_t *px, 
        cfixed16_t *px_cbuf_beg,
        uint32_t   *pscr_buf,
        size_t     cbuf_size, 
        uint32_t   isInitBuffer); 


extern void sss_gen_hp_v1(uint32_t *pScrBuf, fixed16_t *pSeq, uint32_t id1, uint32_t id2);

extern uint32_t sssdet_targeted_hpgemv_v1( cfloat16_t *px, uint32_t *pScrBuf, fixed16_t *pSeq, uint32_t id1, uint32_t id2 );

extern uint32_t sssdet_hpgemv_v1( cfloat16_t *px, uint32_t *pScrBuf, uint32_t id2 );

extern void decim2x_padd_asm( 
        cfixed16_t *out_p,
        cfixed16_t *inp_p,
        cfixed16_t *inp_circ_p,
        cfixed16_t *out_circ_p,
        cfixed16_t *hist_buf,
        size_t      inp_circ_size,
        uint32_t    num_out_lines,
        size_t 		out_circ_size
    );


extern void csp_csp_chf_vmult_vecalgn_asm(cfloat32_t *y,  cfloat32_t *x1,  cfixed16_t *x2, size_t L);
extern void chf_chf_chf_vmult_vecalgn_asm(cfixed16_t *y, cfixed16_t *x1, cfixed16_t *x2, size_t L);

#pragma cplusplus on
static inline void vmult_vecalgn(cfloat32_t* py,  cfloat32_t* px1,   cfixed16_t* px2, size_t L) { csp_csp_chf_vmult_vecalgn_asm(py, px1, px2, L); }
static inline void vmult_vecalgn(cfixed16_t* py, cfixed16_t* px1,  cfixed16_t* px2, size_t L) { chf_chf_chf_vmult_vecalgn_asm(py, px1, px2, L); }


#pragma cplusplus off
extern void  xcn_hf_asm_modified(float *xcn, cfixed16_t *y, float *peakVal, uint32_t *peakIdx);
extern float power_sum_256_sp_asm(float*px,cfixed16_t *pBuff,size_t sz_cbuf);
extern float sum_512_hfx_power_asm(cfixed16_t*px,cfixed16_t *pBuff,size_t sz_cbuf);
extern float noise_estimation_asm(cfixed16_t*px, cfixed16_t *pBuff, float* px2);

#endif // __nr_pss__
