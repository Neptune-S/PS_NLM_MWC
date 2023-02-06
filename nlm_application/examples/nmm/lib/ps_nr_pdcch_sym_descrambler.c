#include "math.h"
#include <stdint.h>
#include <stdio.h>
#include "ps_nr_pdcch.h"

void ps_nr_pdcch_sym_descrambler(int16_t* llrDeScr, int16_t* llr, uint32_t E, uint8_t* pScrCw)
{
    uint16_t i;
    int8_t   cw[2] = { 1, -1 };

    for (i = 0; i < E; i++)
    {
        llrDeScr[i] = llr[i] * cw[pScrCw[i]];
    }
}