
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include "math.h"

void ps_nr_pseudo_random_generator_init(uint32_t cInit, uint32_t *pnX1,uint32_t *pnX2)
{   
    uint8_t goldSeqLen = 31;
    uint16_t NC        = 1600;

    pnX1[0] = 1;
    pnX1[1] = 0;

    pnX2[0] = cInit;
    pnX2[1] = 0;

    uint32_t i = 0;
    uint8_t j  = 0;
    uint8_t k  = j + 3;

    uint8_t l  = j + 1;
    uint8_t m  = j + 2;

    uint8_t s  = j + goldSeqLen;
    uint8_t p  = 0, n = 0, q = 0;
    
    for (i = 0; i <= (NC - goldSeqLen);)
    {

        s = s & 0x3F;

        j = j & 0x3F;
        k = k & 0x3F;

        l = l & 0x3F;
        m = m & 0x3F;
        
        p = s>>5; 
	    n = k >> 5;
	    q = j >> 5;

	    pnX1[p] = pnX1[p] & ~(1 << (s & 0x1F));                                                                                  //reset that bit before setting
        pnX1[p] = pnX1[p] | ((((pnX1[q] >> (j & 0x1F)) & 0x1) + ((pnX1[n] >> (k & 0x1F)) & 0x1)) & 0x1) << (s & 0x1F); //mod 2 operation

        pnX2[p] = pnX2[p] & ~(1 << (s & 0x1F));                                                                                                                                                          //reset that bit
        pnX2[p] = pnX2[p] | ((((pnX2[q] >> (j & 0x1F)) & 0x1) + ((pnX2[n] >> (k & 0x1F)) & 0x1) + ((pnX2[l >> 5] >> (l & 0x1F)) & 0x1) + ((pnX2[m >> 5]) >> (m & 0x1F))) & 0x1) << (s & 0x1F); //mod 2 operation

        j++; 
        k++;

        l++;
        m++;

        s++;
        i++;
    }
}

void ps_nr_pseudo_random_generator(uint32_t seqLen ,uint8_t* c ,uint8_t ssbIndex, uint32_t *pnX1, uint32_t *pnX2)
{
    uint8_t j = 34, k = 37, l = 35, m = 36, s = 1;
    uint8_t goldSeqLen = 31;
    uint8_t idxFlag = 0;
    uint16_t i = 0;
    uint32_t pnTempX1[2];
    uint32_t pnTempX2[2];
     uint8_t p  = 0, n = 0, q = 0;

    pnTempX1[0] = pnX1[0];
    pnTempX1[1] = pnX1[1];

    pnTempX2[0] = pnX2[0];
    pnTempX2[1] = pnX2[1];

    for (i = 0; i < (ssbIndex * seqLen);)
    {
        s = s & 0x3F;

        j = j & 0x3F;
        k = k & 0x3F;

        l = l & 0x3F;
        m = m & 0x3F;

	    p = s >> 5;
        n = k >> 5;
        q = j >> 5;

        pnTempX1[p] = pnTempX1[p] & ~(1 << (s & 0x1F));                                                                                          //reset that bit before setting
        pnTempX1[p] = pnTempX1[p] | ((((pnTempX1[q] >> (j & 0x1F)) & 0x1) + ((pnTempX1[n] >> (k & 0x1F)) & 0x1)) & 0x1) << (s & 0x1F); //mod 2 operation

        pnTempX2[p] = pnTempX2[p] & ~(1 << (s & 0x1F));                                                                                                                                                                          //reset that bit
        pnTempX2[p] = pnTempX2[p] | ((((pnTempX2[q] >> (j & 0x1F)) & 0x1) + ((pnTempX2[n] >> (k & 0x1F)) & 0x1) + ((pnTempX2[l >> 5] >> (l & 0x1F)) & 0x1) + ((pnTempX2[m >> 5]) >> (m & 0x1F))) & 0x1) << (s & 0x1F); //mod 2 operation

        j = j + 1;
        k = k + 1;

        l = l + 1;
        m = m + 1;

        s = s + 1;
        i = i + 1;
    }

    idxFlag = (s >= 32) ? 0 : 1;

    pnTempX1[idxFlag] = pnTempX1[idxFlag] >> 1;
    pnTempX2[idxFlag] = pnTempX2[idxFlag] >> 1;

    uint8_t c1[864+31];
    uint8_t c2[864+31];

    for (i = 0; i < seqLen; i++)
    {
        if (i < 28)
        {
            c1[i] = (pnTempX1[idxFlag] >> i) & 0x1;
            c1[i + 3] = (pnTempX1[idxFlag] >> (i + 3)) & 0x1;

            c2[i] = (pnTempX2[idxFlag] >> i) & 0x1;
            c2[i + 1] = (pnTempX2[idxFlag] >> (i + 1)) & 0x1;
            c2[i + 2] = (pnTempX2[idxFlag] >> (i + 2)) & 0x1;
            c2[i + 3] = (pnTempX2[idxFlag] >> (i + 3)) & 0x1;
        }
		
        c1[goldSeqLen + i] = (c1[i] + c1[i + 3]) & 0x1;
        c2[goldSeqLen + i] = (c2[i] + c2[i + 1] + c2[i + 2] + c2[i + 3]) & 0x1;

        c[i] = (c1[goldSeqLen + i] + c2[goldSeqLen + i]) & 0x1;
    }

}
