#include "math.h"
#include <stdint.h>
#include <stdio.h>
#include "armral.h"
#include "ps_nr_pdcch.h"

void ps_nr_qpsk_demodulation(armral_cmplx_int16_t* eqDataRe, uint32_t E, int16_t* llr)
{
	uint16_t i;
	uint16_t T = E >> 1;

	for (i = 0; i < T; i++)
	{
		llr[i << 1] = eqDataRe[i].re;
		llr[(i << 1) + 1] = eqDataRe[i].im;
	}
}
