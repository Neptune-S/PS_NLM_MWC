/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2021-2022 NXP
 */

#ifndef __NMM_DL_PROC_H
#define __NMM_DL_PROC_H

/* TODO: Doxy style comments! */
typedef struct vspa2_complex_fixed16 {
	int16_t real;
	int16_t imag;
} cfixed16_t;

/* MIB data structures */
typedef struct {
	uint8_t bw;
	uint8_t extend_phich;
	uint8_t ng_value;
	uint8_t sfn;
	uint16_t reserved;
} mib_out_T;

typedef struct {
    float   bw_MHz;
    char    phich[4];
    uint8_t num_tx;
    uint8_t minor_sfn;
} mib_extra_T;

/* FFT functions */
int dl_proc_setup(void);
void dl_proc_clean(void);

/* Main processing functions*/
int mib_process(cfixed16_t *input, uint16_t PCID, mib_out_T *mib_data, mib_extra_T *extra, uint8_t *mib_raw);
int sib_process(cfixed16_t *input, uint16_t PCID, uint8_t window_length, uint8_t sf_start, uint32_t frame_num, mib_out_T *mib_data, mib_extra_T *extra, char *sib, size_t *sib_len);

#endif /* __NMM_DL_PROC_H */
