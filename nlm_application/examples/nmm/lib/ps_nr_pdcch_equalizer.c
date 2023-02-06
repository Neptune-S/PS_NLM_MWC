#include <sys/types.h>
#include "armral.h"
#include <string.h>
#include "ps_nr_pdcch.h"

void ps_nr_pdcch_equalizer(armral_cmplx_int16_t *in_data_re,armral_cmplx_int16_t *chan_est_re,armral_cmplx_int16_t *eq_data_re, int16_t n_re)
{
 
 /* first calculate the conjugate */

 for(uint16_t n = 0; n<n_re; n++)
  chan_est_re[n].im = -chan_est_re[n].im;

 /* Now do the equalization */
 armral_cmplx_vecmul_i16(n_re,in_data_re,chan_est_re,eq_data_re);

}  