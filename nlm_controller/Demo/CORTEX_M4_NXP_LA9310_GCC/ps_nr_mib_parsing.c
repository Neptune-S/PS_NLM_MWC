

#include "FreeRTOS.h"
#include "task.h"
#include "pbch.h"
#include "string.h"
#include "pbch.h"
#include "nmm_host_if.h"

void ps_nr_mib_parsing(uint32_t pbchPaylod,mib_info_t * mib_info)
{
    
    uint8_t payload_bit1,payload_bit2,payload_bit3,payload_bit4,payload_bit5,payload_bit6,
    payload_bit7,payload_bit8,payload_bit9,payload_bit10,payload_bit11,payload_bit12,payload_bit13,
    payload_bit14,payload_bit15,payload_bit16,payload_bit17,payload_bit18,payload_bit19,payload_bit20,
    payload_bit21,payload_bit22,payload_bit23,payload_bit24,payload_bit25,payload_bit26,payload_bit27,payload_bit28,payload_bit29;
    
    //Bit Positioning
  //  payload_bit0 = (pbchPaylod>>0)&0x1;
    payload_bit1 = (pbchPaylod>>1)&0x1;
    payload_bit2 = (pbchPaylod>>2)&0x1;
    payload_bit3 = (pbchPaylod>>3)&0x1;
    payload_bit4 = (pbchPaylod>>4)&0x1;
    payload_bit5 = (pbchPaylod>>5)&0x1;
    payload_bit6 = (pbchPaylod>>6)&0x1;  
    payload_bit7 = (pbchPaylod>>7)&0x1;
    payload_bit8 = (pbchPaylod>>8)&0x1;
    payload_bit9 = (pbchPaylod>>9)&0x1;
    payload_bit10 = (pbchPaylod>>10)&0x1;
    payload_bit11 = (pbchPaylod>>11)&0x1;
    payload_bit12 = (pbchPaylod>>12)&0x1;
    payload_bit13 = (pbchPaylod>>13)&0x1;
    payload_bit14 = (pbchPaylod>>14)&0x1;
    payload_bit15 = (pbchPaylod>>15)&0x1;
    payload_bit16 = (pbchPaylod>>16)&0x1;
    payload_bit17 = (pbchPaylod>>17)&0x1;
    payload_bit18 = (pbchPaylod>>18)&0x1;
    payload_bit19 = (pbchPaylod>>19)&0x1;
    payload_bit20 = (pbchPaylod>>20)&0x1;
    payload_bit21 = (pbchPaylod>>21)&0x1;
    payload_bit22 = (pbchPaylod>>22)&0x1;
    payload_bit23 = (pbchPaylod>>23)&0x1;
    payload_bit24 = (pbchPaylod>>24)&0x1;
    payload_bit25 = (pbchPaylod>>25)&0x1;
    payload_bit26 = (pbchPaylod>>26)&0x1;
    payload_bit27 = (pbchPaylod>>27)&0x1;
    payload_bit28 = (pbchPaylod>>28)&0x1;
    payload_bit29 = (pbchPaylod>>29)&0x1;
    
    mib_info->systemFrameNumber |= ((payload_bit1<<5) | (payload_bit2<<4) | (payload_bit3<<3)
              | (payload_bit4<<2) | (payload_bit5<<1) | (payload_bit6<<0));
    mib_info->subCarrierSpacingCommon = payload_bit7;

    mib_info->ssb_subcarrierOffset = (payload_bit8|(payload_bit9<<1)|(payload_bit10<<2)|(payload_bit11<<3));

    mib_info->dmrs_typeA_Position = payload_bit12; 
    
    mib_info->pdcch_configSIB1 |= ((payload_bit13<<7) | (payload_bit14<<6) | (payload_bit15<<5)
                            | (payload_bit16<<4) | (payload_bit17<<3) | (payload_bit18<<2) | (payload_bit19<<1) | (payload_bit20));      
    mib_info->cellBarred = payload_bit21;

    mib_info->intraFreqReselection = payload_bit22;

   
    mib_info->spare = payload_bit23; 

    mib_info->sfn |= (payload_bit6<<0 | payload_bit5<<1 | payload_bit4<<2 | payload_bit3<<3 |
    payload_bit2<<4 | payload_bit1<<5 | payload_bit27<<6 | payload_bit26<<7 | payload_bit25<<8 | payload_bit24<<9);
    
    mib_info->half_frame_bit = payload_bit28; 

    mib_info->k_ssb_msb      = payload_bit29; 

   // printf("systemFrameNumber:%d\nsubCarrierSpacingCommon:%s\nssb_subcarrierOffset:%d\ndmrs_typeA_Position:%s\npdcch_configSIB1:%d\ncellBarred:%s\nintraFreqReselection:%s\nspare:%d\nsfn:%d\nhalf_frame_bit:%d\nk_ssb_msb:%d\n",mib_info->systemFrameNumber,mib_info->subCarrierSpacingCommon,mib_info->ssb_subcarrierOffset,mib_info->dmrs_typeA_Position,
//    mib_info->pdcch_configSIB1,mib_info->cellBarred,mib_info->intraFreqReselection,mib_info->spare,mib_info->sfn,mib_info->half_frame_bit,mib_info->k_ssb_msb);
}
