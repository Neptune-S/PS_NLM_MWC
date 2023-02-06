#include <sys/types.h>
#include "armral.h"
#include <string.h>
#include "ps_nr_pdcch.h"
#include <math.h>
#include <stdio.h>
void ps_nr_pdcch_channel_estimator(armral_cmplx_int16_t *dmrs_re,armral_cmplx_int16_t *dmrs_ref_re,armral_cmplx_int16_t *chan_est_re, void* scratchBuf,int16_t n_re)
{
    double   temp;
    double   n_bits;
    int16_t  real_re;
    int16_t  imag_re;
    uint8_t  shift_bits;

    temp = 0;

    armral_cmplx_int16_t *scratch_buf = (armral_cmplx_int16_t *)scratchBuf;

    /* assuming DMRS REF RE are full 16 bit populated */
    /* We have to calculate a scaling factor to shift the channel estimates to full 16 bits. This will ensure that equalizer doesnot give 0 output. Sort of like AGC */ 

    /* First do the dmrs reference conjugate */
    for(uint16_t n = 0; n<n_re; n++)
    {
        dmrs_ref_re[n].im = -dmrs_ref_re[n].im;
        real_re = dmrs_re[n].re;
        imag_re = dmrs_re[n].im;
        temp += (double)((real_re*real_re) + (imag_re*imag_re));
    }

    temp  = sqrt(temp/n_re);

    n_bits = ceil(log2(temp));

    shift_bits = (uint8_t)(14-n_bits);
    
    /* Now remove the dmrs cover */
    armral_cmplx_vecmul_i16(n_re,dmrs_re,dmrs_ref_re,scratch_buf);
    
    /* Now create the channel estimate */
    for(uint16_t n = 0; n<n_re*N_DMRS_RE_PDCCH; n++)
    {   
        uint8_t k = n/N_DMRS_RE_PDCCH;
        chan_est_re[n].re = (scratch_buf[k].re<<shift_bits);
        chan_est_re[n].im = (scratch_buf[k].im<<shift_bits);          
    }

}  