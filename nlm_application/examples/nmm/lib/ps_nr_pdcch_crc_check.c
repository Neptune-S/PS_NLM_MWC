#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ps_nr_pdcch.h"

uint32_t reverseBits(uint32_t num)
{
  uint32_t count = sizeof(num) * 8 - 1;
  uint32_t reverse_num = num;

  num >>= 1;
  while (num) {
    reverse_num <<= 1;
    reverse_num |= num & 1;
    num >>= 1;
    count--;
  }
  reverse_num <<= count;
  return reverse_num;
}

bool ps_nr_pdcch_crc_check( uint32_t* payload, uint32_t rnti, uint32_t K)
{
 uint8_t L = 24;
 uint32_t crcPoly = 0xD9588B80;
 rnti = rnti << 8; /*0x0000ffff to 0x00ffff00, since we're using first 24 bits of MSB*/

 /*crcPoly = [1 1 0 1 1 0 0 1 0 1 0 1 1 0 0 0 1 0 0 0 1 0 1 1 1 ]; #24C, rnti = 65535;*/

 payload[0] = reverseBits(payload[0]);
 payload[1] = reverseBits(payload[1]);

 uint32_t rxCrc = (((payload[1] >> (64 - K)) & 0x00FFFFFF) << 8); 

 uint32_t aDash[2] = {0xffffff00,0x00000000};
 aDash[0] |= (payload[0]>>24);
 aDash[1] |= ((payload[0]&0x00ffffff)<<8);
 aDash[1] |= (((payload[1] >> (64 - K))>>L) << ( 64-K));
 
 uint32_t input[3] = {aDash[0], aDash[1], 0x00000000};
 
 uint32_t dataShift = (((aDash[0] >> (32-L)) & 0x00FFFFFF) << (31-L)) ;
 uint8_t i = 1;

 while(i<=K)
 {
   uint8_t n = i+L-1;
   uint8_t index = (n) >> 5;
   uint8_t offset = ((index+1)<<5) - n-1;
   uint32_t bitOut = 0x00000000;
   bitOut |= ( (dataShift << 1)|(((input[index]>>(offset))& 0x1) << (31-L)));
   dataShift = bitOut;
   if((bitOut>>31) == 0x1)
   {
     dataShift = (crcPoly ^ bitOut);
   } 
   i++;
 }
 uint32_t calCrc = (dataShift << 1);
 calCrc = (calCrc  ^ rnti);
 bool crcPassFlag = 0;
 if( calCrc != 0 && rxCrc !=0 && calCrc == rxCrc) crcPassFlag  = true;
 else crcPassFlag  = false;

 return crcPassFlag;
}