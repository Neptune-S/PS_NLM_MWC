function coreset_sym_rbs = ps_nr_pdcch_coreset_extraction(rx_data,nSym,symb_offset,rb_offset,rbs,kssb)
  
 cp_first_symbol = 88;
 cp_other_symbol = 72;
 fs              = 30.72e+6;
 
 cp_length_type_0 = [88,72,72];
 cp_length_type_1 = [72,72,72];
 
 fftSize            = 1024;
 n_re_rb            = 12;
 ssb_rbs            = 20;
 kssb_scs           = 15e+3;
 
 start_rb_second_half  = ssb_rbs/2+rb_offset; % This is the offset from the DC
 start_re_second_half  = fftSize - start_rb_second_half*n_re_rb+1;
 end_rb_second_half    = (rbs - start_rb_second_half)*n_re_rb;
 
 coreset_sym_rbs = [];
 
 crb_alignment_freq_shift = kssb_scs*kssb;
 
 rx_data1            = rx_data.*exp(1j.*(2.*pi./fs).*crb_alignment_freq_shift.*(0:length(rx_data)-1).');
  
 if(symb_offset == 0)
   sym_offset = 1;
   for n = 1:nSym
     cp_length       = cp_length_type_0(n);
     jitt_gaurd      = cp_length/4;
     phase_derot_jitt_gaurd   = exp(j*2*pi*(cp_length-jitt_gaurd).*[0:fftSize-1].'./fftSize);
     rx_symb_fft     = fft(rx_data1(sym_offset + jitt_gaurd + (0:fftSize-1))).*phase_derot_jitt_gaurd;
     re_first_half   = rx_symb_fft(start_re_second_half:end);
     re_second_half  = rx_symb_fft(1:end_rb_second_half);
     coreset_sym_rbs = [coreset_sym_rbs;[re_first_half;re_second_half]];
     sym_offset      = sym_offset+cp_length+fftSize;      
   end
  else
   sym_offset = (cp_length_type_0(1)+fftSize) + (symb_offset-1)*(cp_length_type_0(2)+fftSize)+1;
   for n = 1:nSym
     cp_length       = cp_length_type_1(n);
     jitt_gaurd      = cp_length/4;
     phase_derot_jitt_gaurd   = exp(j*2*pi*(cp_length-jitt_gaurd).*[0:fftSize-1].'./fftSize);
     rx_symb_fft     = fft(rx_data1(sym_offset + jitt_gaurd + (0:fftSize-1))).*phase_derot_jitt_gaurd;
     re_first_half   = rx_symb_fft(start_re_second_half:end);
     re_second_half  = rx_symb_fft(1:end_rb_second_half);
     coreset_sym_rbs = [coreset_sym_rbs;[re_first_half;re_second_half]];
     sym_offset     = sym_offset+cp_length+fftSize;     
   end
 end
 
end