
#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include <string.h>

uint32_t ps_nr_pbch_bit_deinterleaver(uint32_t bitIn){
    
    uint32_t bitOut = 0; 
    uint8_t i = 0;
    
    uint8_t pbchInterleaverPattern[32] = {16,23,18,17,8,30,10,6,24,7,0,5,3,2,1,4,9,11,12,13,14,15,19,20,21,22,25,26,27,28,29,31};
    
    // First 6 SFN Bits
    uint8_t jsfn    = 0;
    uint8_t jother  = 14;
    uint8_t jhrf    = 10;
    uint8_t jssb    = 11;
    
    bitOut |= (bitIn>>pbchInterleaverPattern[jother])&0x1;
    i = i+1;
    jother++;

    for(i=1;i<7;i++){
      bitOut |= ((bitIn>>pbchInterleaverPattern[jsfn])&0x1)<<i;
      jsfn++;
    }

    for(i=7;i<24;i++){
      bitOut |= ((bitIn>>pbchInterleaverPattern[jother])&0x1)<<i;
      jother++;
    }
    
    for(i=24;i<28;i++){
      bitOut |= ((bitIn>>pbchInterleaverPattern[jsfn])&0x1)<<i;
      jsfn++;
    }

    bitOut |= ((bitIn>>pbchInterleaverPattern[jhrf])&0x1)<<i;

    for(i=29;i<32;i++){
      bitOut |= ((bitIn>>pbchInterleaverPattern[jssb])&0x1)<<i;
      jssb++;
    }
    
    return bitOut;
}
