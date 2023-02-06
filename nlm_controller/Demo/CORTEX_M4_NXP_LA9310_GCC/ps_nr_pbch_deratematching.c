
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include "math.h"


void ps_nr_pbch_deratematch_indices(uint16_t *pbchDeRateMatchIndices)
{   
    uint16_t N = 512;
    uint8_t  j = 0;
    uint16_t i;
//    uint8_t fact  = 16; //N >> 5;
    //const uint8_t P[32]  = {0,1,2,4,3,5,6,7,8,16,9,17,10,18,11,19,12,20,13,21,14,22,15,23,24,25,26,28,27,29,30,31}; 
    const uint16_t P[32] = { 0,16,32,64,48,80,96, 112, 128, 256, 144, 272, 160, 288, 176, 304, 192, 320, 208, 336, 224, 352, 240, 368, 384, 400, 416, 448, 432, 464, 480, 496}; 
    for(i = 0; i < N ; i++)
    {
        j = (i >> 4); //(32*i/512)
        pbchDeRateMatchIndices[i] = P[j] + (i&0xF);
    }
}

void ps_nr_pbch_deratematching(int16_t* llrIn, float* llrOut, uint16_t * pbchDeRateMatchIndices)
{    
    uint16_t N = 512;
    uint16_t E = 864;
    uint16_t i;
    uint16_t j = 0;
    float temp[352];     //E-N are averaged so float

    for (i = N; i < E; i++)
    {
        temp[j] = (llrIn[j] + llrIn[i])*0.5; //divide by 2
        j++;
    }

    for(i = 0; i < (E-N) ; i++)
    {
        llrOut[pbchDeRateMatchIndices[i]] = temp[i];
    }
    
    for(i = E-N; i < N ; i++)
    {
        llrOut[pbchDeRateMatchIndices[i]] = (float)(llrIn[i]);
    }    
}
