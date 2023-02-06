#include "FreeRTOS.h"
#include "task.h"
#include "debug_console.h"
#include "pbch.h"
#include "math.h"

void ps_nr_pbch_re_demapper(int16_t *fftData, cplx_fxp_int * dmrsRe, cplx_fxp_int* dataRe,uint16_t cellId)
{
    uint16_t fftSize     = 512;
   // uint16_t nPBCHSub    = 240;
    uint16_t dmrsSpacing = 4;
    //uint16_t subStart;
   // uint16_t subEnd;
    uint16_t v;
    uint16_t k;
    uint16_t m;
    uint16_t nPBCHdrmsLoc; 
    uint16_t idy;
    uint16_t offsetDmrs;
    uint16_t offsetData;
    uint16_t offset;
    uint16_t offsetk;
    
    uint16_t subIdxBitRev_high[120] = {35,291,163,419,99,355,227,483,19,275,147,403,83,339,211,467,51,307,179,435,115,371,243,499,11,267,139,395,75,331,203,459,43,299,171,427,107,363,235,491,27,283,155,411,91,347,219,475,59,315,187,443,123,379,251,507,7,263,135,391,71,327,199,455,39,295,167,423,103,359,231,487,23,279,151,407,87,343,215,471,55,311,183,439,119,375,247,503,15,271,143,399,79,335,207,463,47,303,175,431,111,367,239,495,31,287,159,415,95,351,223,479,63,319,191,447,127,383,255,511};
    uint16_t subIdxBitRev_low[120]  = {0,256,128,384,64,320,192,448,32,288,160,416,96,352,224,480,16,272,144,400,80,336,208,464,48,304,176,432,112,368,240,496,8,264,136,392,72,328,200,456,40,296,168,424,104,360,232,488,24,280,152,408,88,344,216,472,56,312,184,440,120,376,248,504,4,260,132,388,68,324,196,452,36,292,164,420,100,356,228,484,20,276,148,404,84,340,212,468,52,308,180,436,116,372,244,500,12,268,140,396,76,332,204,460,44,300,172,428,108,364,236,492,28,284,156,412,92,348,220,476};
    
    v = (cellId%4);
    
    nPBCHdrmsLoc = v;
    k = 0;
    m = 0;
    /* Processing first and third PBCH symbol */
    offsetDmrs = 84;
    offsetData = 252;
    offset     = (fftSize<<2);
    for(uint16_t idx = 392;idx<512;idx++)
    {    
        idy = subIdxBitRev_high[idx-392]<<1;
        
        if ((idx-392) == nPBCHdrmsLoc)
        {   
            offsetk                   = k+offsetDmrs;
            dmrsRe[k].real            = fftData[idy];
            dmrsRe[offsetk].real = fftData[idy+offset];
           
            idy++;
            
            dmrsRe[k].imag            = fftData[idy];
            dmrsRe[offsetk].imag = fftData[idy+offset];
            k++;
            nPBCHdrmsLoc += dmrsSpacing;
        }else
        {   
            offsetk                   = m+offsetData;
            dataRe[m].real            = fftData[idy];
            dataRe[offsetk].real      = fftData[idy+offset];
            
            idy++;
            
            dataRe[m].imag            = fftData[idy];
            dataRe[offsetk].imag      = fftData[idy+offset];
            m++;
        }
    }
   
    for(uint16_t idx = 0;idx<120;idx++)
    {   
        idy = subIdxBitRev_low[idx]<<1;
        
        if ((idx+120) == nPBCHdrmsLoc)
        {   
            offsetk                    = k+offsetDmrs;
            
            dmrsRe[k].real            = fftData[idy];
            dmrsRe[offsetk].real      = fftData[idy+offset];
            
            idy++;
            
            dmrsRe[k].imag            = fftData[idy];
            dmrsRe[offsetk].imag      = fftData[idy+offset];
            
            k++;
            
            nPBCHdrmsLoc += dmrsSpacing;
        }else
        {   
            offsetk                   = m+offsetData;
            dataRe[m].real            = fftData[idy];
            dataRe[offsetk].real      = fftData[idy+offset];
            
            idy++;
            
            dataRe[m].imag            = fftData[idy];
            dataRe[offsetk].imag      = fftData[idy+offset];
            m++;
            
        }
    }
    
    offsetDmrs   = 60;
    offsetData   = 180;
    offset       = (fftSize<<1);
    nPBCHdrmsLoc = v;
    k = 0;
    m = 0;
    /* Processing 2nd OFDM symbol */
    
    for(uint16_t idx = 392;idx<440;idx++)
    {   
        idy = subIdxBitRev_high[idx-392]<<1;
        
        if ((idx-392) == nPBCHdrmsLoc)
        {   
            offsetk = k+offsetDmrs;
            
            dmrsRe[offsetk].real = fftData[idy+offset];
            
            idy++;
            
            dmrsRe[offsetk].imag = fftData[idy+offset];
            
            k++;
            nPBCHdrmsLoc += dmrsSpacing;
        }else
        {   
            offsetk = m+offsetData;
            
            dataRe[offsetk].real = fftData[idy+offset];
            
            idy++;
            
            dataRe[offsetk].imag = fftData[idy+offset];
            
            m++;
        }
    }
    
    nPBCHdrmsLoc = v;
    
    for(uint16_t idx = 72;idx<120;idx++)
    {  
        idy = subIdxBitRev_low[idx]<<1;
    
        if ((idx-72) == nPBCHdrmsLoc)
        {   
            
            offsetk = k+offsetDmrs;
            
            dmrsRe[offsetk].real = fftData[idy+offset];
            
            idy++;
            
            dmrsRe[offsetk].imag = fftData[idy+offset];
            
            k++;
            
            nPBCHdrmsLoc += dmrsSpacing;
            
        }else
        {   
            offsetk = m+offsetData;
            
            dataRe[offsetk].real = fftData[idy+offset];
            
            idy++;
            
            dataRe[offsetk].imag = fftData[idy+offset];
            
            m++;
            
        }
    }
} 