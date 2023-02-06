function [K, E, N] = ps_nr_pdcch_calculate_K_N_E(aggrLvl, coreset_rbs)
    
  crcLen = 24;
  pdcchFixedPayloadLen = 28;
  qpskBits = 2;
  
  dataScBitsCount = aggrLvl*6*12*0.75*qpskBits; #0.75 = 3/4   
  E = dataScBitsCount;
  #38.212 Page-14
  K = ceil(log2(coreset_rbs * (coreset_rbs+1)/2)) + pdcchFixedPayloadLen + crcLen;
  nmin = 5;
  nmax = 9;
  n1 = ceil(log2(E));
  
  if(E <= ((9/8)*pow2(ceil(log2(E) -1))))
    if(K/E < 9/16)
      n1 = ceil(log2(E)) - 1 ;
    endif
   endif
   
   Rmin = 1/8;
   n2 = ceil(log2(K/Rmin));
   n = max(min(n1,min(n2, nmax)), nmin);
   
   N = pow2(n);
   
  endfunction