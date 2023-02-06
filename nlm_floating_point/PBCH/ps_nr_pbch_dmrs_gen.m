function dmrsRefOut  = ps_nr_pbch_dmrs_gen(cellId)
   
  if ((cellId > 1007) || (cellId < 0))
    disp("Invalid CellId");
  endif
    
  dmrsREs = 144;
  scalingFactor = 1/sqrt(2);
  pnLength = dmrsREs * 2;
  
  for ssbIndex = 0:1:7
  cInit = (2^11)*(ssbIndex + 1)*(floor(cellId/4) + 1) + (2^6)*(ssbIndex + 1) + mod(cellId, 4);
  c = ps_nr_pseudo_random_generator(cInit, pnLength);  
  dmrsRefOut(1:dmrsREs, ssbIndex + 1) = scalingFactor.*((1- 2.*c(1:2:end-1)) + 1j*(1 - 2.*c(2:2:end)));
  endfor

endfunction