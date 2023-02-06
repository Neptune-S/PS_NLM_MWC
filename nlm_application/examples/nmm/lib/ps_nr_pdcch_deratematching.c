#include <math.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include "ps_nr_pdcch.h"

void ps_nr_pdcch_deratematching(int16_t* llr, float* llrOut, uint16_t N, uint16_t E, uint16_t K, void* scratchBuf)
{
    int16_t* temp = (int16_t*)scratchBuf;

    uint16_t R = floor(E / N);
    uint16_t repeat;
    uint16_t k;
    uint16_t m = R * N;
    uint16_t n;
    uint16_t S;


    memset(temp, 0, 1728*sizeof(int16_t));

    // repetition case
    if (E >= N)
    {
        for (repeat = 1; repeat <= R; repeat++) {
            n = repeat - 1;
            S = n * N;
            for (k = 0; k < N; k++) {
                temp[k] += llr[S + k] / R;
            }
        }
        for (k = m; k < E; k++) {
            temp[k - m] = (llr[k] + temp[k - m]) >> 1; 
        }
    }
    // punctured case    
    else if ((K / E) <= 7 / 16) {
        for (k = 0; k < E; k++) {
            temp[k + N - E] = llr[k];
        }
    }

    // shortened case
    else {
        for (k = 0; k < E; k++) {
            temp[k] = llr[k];
        }
        for (k = E; k < N; k++) {
            temp[k] = MAX_INT_16;
        }
    }


    // interleaving
    uint16_t  j = 0;
    uint16_t i;
    uint16_t fact = N >> 5;
    uint16_t shift = log2(fact);
    const uint8_t P[32] = { 0,1,2,4,3,5,6,7,8,16,9,17,10,18,11,19,12,20,13,21,14,22,15,23,24,25,26,28,27,29,30,31 };
    uint16_t pbchDeRateMatchIndices[N];
    for (i = 0; i < N; i++)
    {
        j = i >> shift;
        pbchDeRateMatchIndices[i] = (P[j] << shift) + (i - ((i >> shift) << shift));
        llrOut[pbchDeRateMatchIndices[i]] = (float)temp[i];
    }

}