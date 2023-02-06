#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ps_nr_pdcch.h"
#include "armral.h"
#include <rte_memcpy.h>

void ps_nr_pdcch_gen_rb_indices(uint8_t* cceRegInterleaverIndices, uint8_t aggrLevel, uint8_t numCoresetSym, uint8_t numCoresetRBs, uint8_t* cceCandidates, uint8_t* rbCandPerAgg)
{
    uint8_t startRegCandPerAgg[MAX_PDCCH_AGGREGATION];
    uint8_t numRegPerCcePerSym = NCCE / numCoresetSym;
    uint8_t i=0;

    for (i = 0; i < aggrLevel; i++)
    {
        startRegCandPerAgg[i] = cceRegInterleaverIndices[cceCandidates[i]] * numRegPerCcePerSym;
    }

    i = 0;
    for (uint8_t symIdx = 0; symIdx < numCoresetSym; symIdx++)
    {
        for (uint8_t intlIdx = 0; intlIdx < aggrLevel; intlIdx++)
        {
            rbCandPerAgg[i] = startRegCandPerAgg[intlIdx] + (symIdx * numCoresetRBs);

            for (uint8_t cceIdx = 0; cceIdx < numRegPerCcePerSym - 1; cceIdx++)
            {
                i = i + 1;
                rbCandPerAgg[i] = rbCandPerAgg[i - 1] + 1;
            }
            i = i + 1;
        }
    }
 	sort(rbCandPerAgg, (aggrLevel* NCCE));
}

void ps_nr_pdcch_reg_to_re_demapping(uint8_t* rbCandPerAgg,  uint8_t aggrLevel, armral_cmplx_int16_t* pdcchDataSc, armral_cmplx_int16_t* pdcchDmrsSc, armral_cmplx_int16_t* inp)
{
    uint8_t rbIdx = 0, scIdx = 0;
    uint8_t dataScIndices[PDCCH_DATA_SC_PER_RB] = {0, 2, 3, 4, 6, 7, 8, 10, 11};
    uint8_t dmrsScIndices[PDCCH_DMRS_SC_PER_RB] = {1, 5, 9};
    uint16_t tempIndx=0;
    uint16_t dataScWrIdx = 0, dmrsScWrIdx = 0;

    for (rbIdx = 0; rbIdx < (aggrLevel * NCCE); rbIdx++)
    {
        for (scIdx = 0; scIdx < PDCCH_DATA_SC_PER_RB; scIdx++)
        {
          //  pdcchDataScIndices[scIdx] = rbCandPerAgg[rbIdx] * 12 + dataScIndices[scIdx];
              tempIndx = rbCandPerAgg[rbIdx] * 12 + dataScIndices[scIdx];
              rte_memcpy(&pdcchDataSc[dataScWrIdx], &inp[tempIndx],sizeof(armral_cmplx_int16_t));
              dataScWrIdx += 1;
        }

        for (scIdx = 0; scIdx < PDCCH_DMRS_SC_PER_RB; scIdx++)
        {
        //    pdcchDmrsScIndices[scIdx] = rbCandPerAgg[rbIdx] * 12 + dmrsScIndices[scIdx];
            tempIndx = rbCandPerAgg[rbIdx] * 12 + dmrsScIndices[scIdx];
            rte_memcpy(&pdcchDmrsSc[dmrsScWrIdx], &inp[tempIndx],sizeof(armral_cmplx_int16_t));
            dmrsScWrIdx += 1;
        }
    }
}

void swap(uint8_t* a, uint8_t* b)
{
    uint8_t temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void sort(uint8_t *inputIndices, uint8_t len)
{
    uint8_t i=0,j=0;
    for(i=0;i<=len-2;i++)
    {
        for(j=i+1;j<len;j++)
        {
            if(inputIndices[i] > inputIndices[j])
            swap(&inputIndices[i], &inputIndices[j]);
        }
    }
}

void ps_nr_pdcch_gen_cce_candidates(uint8_t aggLvl, uint8_t maxNumCand, uint8_t numCCEs, uint8_t candIdx, uint8_t* cceCandidates)
{
    uint8_t i=0, Y=0, nCI=0;
    
    for(i=0; i < aggLvl; i++)
    {
        cceCandidates[i] =  aggLvl * ((Y +  (candIdx*numCCEs)/(aggLvl*maxNumCand) + nCI) % ((numCCEs/aggLvl))) + i;
    }
}

void ps_nr_pdcch_gen_rb_interleaving_indices(uint8_t numCoresetRBs, uint8_t numCoresetSym, uint16_t cellId, uint8_t* cceRegInterleaving)
{
    uint8_t c= 0, r=0, j=0;
    uint8_t numCoresetReg = (numCoresetRBs * numCoresetSym);
    uint8_t sib1RegBundleSize = 6; //It will change for other SIBs
    uint8_t interleaverSize = 2;
    uint16_t nshift = cellId;
    uint8_t numRegPerBundle = numCoresetReg/sib1RegBundleSize;
    uint8_t C = numCoresetReg/(sib1RegBundleSize * interleaverSize);
    
    for(c = 0; c < C; c++)
    {
        for(r = 0;r < interleaverSize; r++)
        {
            j = (c * interleaverSize + r);
            cceRegInterleaving[j] = (((r*C)+c+nshift)% numRegPerBundle);
        }
    }
}

