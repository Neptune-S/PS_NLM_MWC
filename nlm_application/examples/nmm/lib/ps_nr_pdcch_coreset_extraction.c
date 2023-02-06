#include <sys/types.h>
#include "armral.h"
#include <string.h>
#include "ps_nr_pdcch.h"
#include <rte_memcpy.h>

void ps_nr_pdcch_coreset_extraction(armral_cmplx_int16_t *inSym,uint8_t nSym,uint8_t symb_offset,uint8_t rb_offset,uint8_t rbs,uint8_t kssb,armral_cmplx_int16_t *outSym, void * scratchBuf)
{
 
 armral_fft_direction_t dir = ARMRAL_FFT_FORWARDS; // We are taking FFT here. 
 armral_fft_plan_t *p = NULL;
 armral_fft_create_plan_cs16(&p, 1024, dir);
  
 uint8_t cp_length;
 uint8_t cp_length_type_0[3] = {88,72,72};
 uint8_t cp_length_type_1[3] = {72,72,72};
 
 uint16_t fftSize            = 1024;
 uint8_t n_re_rb             = 12;
 uint8_t ssb_rbs             = 20;
    
 uint16_t start_rb_second_half  = (ssb_rbs>>1) + rb_offset; // This is the offset from the DC
 uint16_t start_re_second_half  = fftSize - start_rb_second_half*n_re_rb;
 uint16_t end_re_first_half     = (rbs - start_rb_second_half)*n_re_rb;

 armral_cmplx_int16_t *rx_symb_fft = (armral_cmplx_int16_t *)scratchBuf;
 /*armral_cmplx_int16_t* phase_derot_jitt_guard = (armral_cmplx_int16_t*)scratchBuf2;
 
 ------------------------------------------------------------redundant now should be deleted----------------------------------------------------------------
 armral_cmplx_int16_t* phase_derot_jitt_guard18 = (armral_cmplx_int16_t*)scratchBuf2; // added
 armral_cmplx_int16_t* phase_derot_jitt_guard22 = (armral_cmplx_int16_t*)scratchBuf3; // added
 armral_cmplx_int16_t *rx_symb_fft2 = (armral_cmplx_int16_t*)scratchBuf4; // added
 armral_cmplx_int16_t* rx_symb_fft3 = (armral_cmplx_int16_t*)scratchBuf5; // added

 uint16_t  exponential[fftSize];    //added
 
 uint8_t jitt_guard = 22;
 for (i = 0; i < fftsize; i++)                                               //added
 {

     //     exponential[i] = 2*M_PI*(cp_length - jitt_guard)/fftSize * i;
     exponential[i] = (M_PI >> 9) * (cp_length - jitt_guard) * i;
     phase_derot_jitt_guard22[i].re = cos(exponential[i]) << 15;
     phase_derot_jitt_guard22[i].im = sin(exponential[i]) << 15;
     exponential[i] = i;
 }

 uint8_t jitt_guard = 18;
 for (i = 0; i < fftsize; i++)                                               //added
 {

     //     exponential[i] = 2*M_PI*(cp_length - jitt_guard)/fftSize * i;
     exponential[i] = (M_PI >> 9) * (cp_length - jitt_guard) * i;
     phase_derot_jitt_guard18[i].re = cos(exponential[i]) << 15;
     phase_derot_jitt_guard18[i].im = sin(exponential[i]) << 15;
     exponential[i] = i;
 }
 ------------------------------------------------------------redundant now should be deleted-------------------------------------------------------------
*/ //redundant now
 if(symb_offset == 0)
 {	
 	
 	uint32_t sym_offset = 0;

  	for(uint8_t n = 0;n<nSym; n++)
  	{	
    	cp_length               = cp_length_type_0[n];
    	uint32_t sym_start_loc  = sym_offset+cp_length;
 /*       uint32_t jitt_guard = cp_length >> 2;                                       //added

        armral_fft_execute_cs16(p, inSym + sym_offset + jitt_guard , rx_symb_fft2);
        
        if (n == 0)
        {
            phase_derot_jitt_guard = phase_derot_jitt_guard_first; // phase_derot_jitt_guard_first declared in header file
        }
        else
        {
            phase_derot_jitt_guard = phase_derot_jitt_guard_rest;  // phase_derot_jitt_guard_rest declared in header file
        }

        armral_cmplx_vecdot_i16(fftSize, rx_symb_fft2, phase_derot_jitt_guard, rx_symb_fft3);

*/
        // Insert FFT here // 
        armral_fft_execute_cs16(p,inSym + sym_start_loc,rx_symb_fft);
    	
    	for(uint16_t re_idx = start_re_second_half; re_idx<fftSize; re_idx++)
		{   
			rx_symb_fft[re_idx].re >>= 5; // FFT will lead to 5 bit of bit growth. Which may saturate the data at peak power
            rx_symb_fft[re_idx].im >>= 5;
    		rte_memcpy(outSym++,&rx_symb_fft[re_idx],sizeof(armral_cmplx_int16_t));
		}
        for(uint16_t re_idx = 0; re_idx<end_re_first_half; re_idx++)
		{   
			rx_symb_fft[re_idx].re >>= 5; // FFT will lead to 5 bit of bit growth. Which may saturate the data at peak power
            rx_symb_fft[re_idx].im >>= 5;
    		rte_memcpy(outSym++,&rx_symb_fft[re_idx],sizeof(armral_cmplx_int16_t));
		} 		
    	sym_offset += cp_length+fftSize;    	    
    }
  } 
  else
  {	
   
   uint16_t sym_offset = cp_length_type_0[0]+fftSize + (cp_length_type_0[1]+fftSize)*(symb_offset-1);
   
   for(uint8_t n=0;n<nSym; n++)
   {	
    	cp_length       = cp_length_type_1[n];
    	uint32_t sym_start_loc  = sym_offset+cp_length;
     
        /*       uint32_t jitt_guard = cp_length >> 2;                                       //added

       armral_fft_execute_cs16(p, inSym + sym_offset + jitt_guard , rx_symb_fft2);

      
       phase_derot_jitt_guard = phase_derot_jitt_guard_rest;  // phase_derot_jitt_guard_rest declared in header file
      
       armral_cmplx_vecdot_i16(fftSize, rx_symb_fft2, phase_derot_jitt_guard, rx_symb_fft3);

*/


        // Insert FFT here //
     	armral_fft_execute_cs16(p,inSym + sym_start_loc,rx_symb_fft);
     
    	for(uint16_t re_idx = start_re_second_half; re_idx<fftSize; re_idx++)
		{   
			rx_symb_fft[re_idx].re >>= 5; // FFT will lead to 5 bit of bit growth. Which may saturate the data at peak power
            rx_symb_fft[re_idx].im >>= 5;
    		rte_memcpy(outSym++,&rx_symb_fft[re_idx],sizeof(armral_cmplx_int16_t));
		}
    	for(uint16_t re_idx = 0; re_idx<end_re_first_half; re_idx++)
		{
			rx_symb_fft[re_idx].re >>= 5; // FFT will lead to 5 bit of bit growth. Which may saturate the data at peak power
            rx_symb_fft[re_idx].im >>= 5;
    		rte_memcpy(outSym++,&rx_symb_fft[re_idx],sizeof(armral_cmplx_int16_t));
		}  
    	symb_offset += cp_length+fftSize;     
   }
  }

  armral_fft_destroy_plan_cs16(&p);


}  