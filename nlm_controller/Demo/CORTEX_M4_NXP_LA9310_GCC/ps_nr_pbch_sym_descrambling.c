
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include "math.h"

void ps_nr_pbch_sym_descrambler(int16_t* llrDeScr, int16_t* llrIn,  uint8_t* pScrCw)
{
    uint16_t i;
    uint16_t seqLen = 864;
    int8_t   cw[2] = {1, -1};
  
    for(i=0;i<seqLen;i++)
    {
        llrDeScr[i] = llrIn[i] * cw[pScrCw[i]];
    }
}
