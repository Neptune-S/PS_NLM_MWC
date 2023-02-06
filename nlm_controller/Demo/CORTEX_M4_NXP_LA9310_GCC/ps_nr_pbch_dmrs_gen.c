
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "timers.h"
#include "phytimer.h"
#include "pbch.h"
#include "math.h"

void ps_nr_pbch_dmrs_gen(cplx_fxp_int* dmrsRefOut, uint16_t cellId)
{
    uint8_t dmrsREs = 144;
    uint16_t seqLen = dmrsREs << 1, j = 0;
    uint32_t cInit;
    uint32_t pnX1[2] = { 0, 0 };
    uint32_t pnX2[2] = { 0, 0 };
    int16_t mod[2] = { 23170, -23170 };
    uint8_t* pScrCw = (uint8_t*)&scratchBuf_pong[0];

    uint8_t reIdx;
    for (int ssbIdx = 0; ssbIdx < 8; ssbIdx++)
    {
        cInit = (1 << 11) * (ssbIdx + 1) * ((cellId >> 2) + 1) + (1 << 6) * (ssbIdx + 1) + (cellId & 0x3);
        ps_nr_pseudo_random_generator_init(cInit, pnX1, pnX2);

        ps_nr_pseudo_random_generator(seqLen, pScrCw, 0, pnX1, pnX2);

        for (reIdx = 0; reIdx < dmrsREs; reIdx++)
        {
            dmrsRefOut[reIdx].real = mod[pScrCw[j++]];
            dmrsRefOut[reIdx].imag = mod[pScrCw[j++]];
        }
        j = 0;
        dmrsRefOut = dmrsRefOut + 144;
    }

}
