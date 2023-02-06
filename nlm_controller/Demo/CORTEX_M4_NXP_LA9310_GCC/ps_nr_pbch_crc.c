#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"


uint32_t ps_nr_pbch_crc(uint32_t *inBits, uint32_t payloadLen)
{
    uint32_t crcPolynomial = 0x1D11A9B;
    uint32_t crcRem        = 0;
    uint32_t nBits         = payloadLen+24;
    uint32_t temp          = 0;

    for(uint32_t n = 0; n<nBits;n++)
    {
        crcRem = crcRem>>1;
        if(n<32)
        {
            crcRem = crcRem|(((*inBits>>n)&0x000001)<<24);
        }else
        {
            crcRem = crcRem|(((temp>>(n-32))&0x000001)<<24);
        }
        
        if((crcRem&0x000001))
        {
           crcRem = crcRem^crcPolynomial;   
        }
        
    }
    return (crcRem>>1);
}
