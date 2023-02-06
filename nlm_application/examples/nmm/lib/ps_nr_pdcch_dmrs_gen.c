
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ps_nr_pdcch.h"
#include "armral.h"


void ps_nr_pdcch_dmrs_gen(armral_cmplx_int16_t* dmrsRefOut, uint16_t cellId, uint16_t slotnum, uint8_t dmrsStartSymNum, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t aggrLvl, uint8_t* rbCandPerAgg, void * scratch_buf)
{
    uint32_t cInit;
    uint32_t pnX1[2] = { 0, 0 };
    uint32_t pnX2[2] = { 0, 0 };
    int16_t mod[2]   = { 23170, -23170 };
    uint8_t *pScrCw  = (uint8_t *)scratch_buf;
    uint8_t numRbPerSym = (aggrLvl*NCCE)/numCoresetSym;
    uint16_t seqLen = (numCoresetRBs * PDCCH_DMRS_SC_PER_RB) << 1;

    uint8_t reIdx=0, rbIdx = 0, scIdx=0;
    uint16_t cwIdx=0;

    for (uint8_t symIdx = dmrsStartSymNum; symIdx < (dmrsStartSymNum + numCoresetSym); symIdx++)
    {
        cInit = (((1 << 17) * (14* slotnum + symIdx + 1) * ((cellId << 1) + 1)) + (cellId << 1)) & 0x7FFFFFFF;

        ps_nr_pseudo_random_generator_init(cInit, pnX1, pnX2);

        ps_nr_pseudo_random_generator(seqLen, pScrCw, 0, pnX1, pnX2);

        for (rbIdx = 0; rbIdx < numRbPerSym; rbIdx++)
        {
            cwIdx = (rbCandPerAgg[rbIdx] * 6);

            for(scIdx=0; scIdx < PDCCH_DMRS_SC_PER_RB; scIdx++)
            {
                dmrsRefOut[reIdx].re = mod[pScrCw[cwIdx++]];
                dmrsRefOut[reIdx++].im = mod[pScrCw[cwIdx++]];
            }
        }
        dmrsRefOut = dmrsRefOut + (numRbPerSym*PDCCH_DMRS_SC_PER_RB);
        reIdx = 0;
    }
}
