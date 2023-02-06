
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include "math.h"

void ps_nr_pbch_bit_descrambling(uint32_t* bitIn, uint8_t numSsbBlocks, uint8_t* pScrCw)
{
    uint8_t  pbchPayloadSize = 32;
    uint8_t  halfIdxBitPos  =  0;
    uint8_t  sfnLsb2BitPos  =  24;
    uint8_t  sfnLsb3BitPos  =  6;
    uint8_t  M, v;
    uint32_t temp, oneBitShift;
   
    if(numSsbBlocks == 64)
        M = pbchPayloadSize - 6;
    else
        M = pbchPayloadSize - 3;
        
    uint8_t i, j =0;
  
    v = (((*bitIn >> sfnLsb3BitPos) & 0x1) << 1) + ((*bitIn >> sfnLsb2BitPos) & 0x1);
    oneBitShift = *bitIn;
    *bitIn = 0;
    
    for(i = 0; i < pbchPayloadSize; i++)
    {
        if((i != halfIdxBitPos) && (i != sfnLsb2BitPos) && (i != sfnLsb3BitPos))
        {
            temp = ((oneBitShift & 0x1) + pScrCw[j+v*M]) & 0x1;
            temp = (temp << (i*temp));
            *bitIn = (*bitIn | temp);
            j = j+1;	
	    }
	    oneBitShift = oneBitShift >> 1;
    }
}