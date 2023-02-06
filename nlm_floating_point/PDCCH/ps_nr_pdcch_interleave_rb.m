function [cceRegInterleaving] = ps_nr_pdcch_interleave_rb(coresetNRB, coresetNysm, cellID)
    
  #NCCE candidates per aggregation Level ##  
  ncoresetReg = coresetNRB * coresetNysm;
  regBundleSize = 6;
  interleaverSize = 2;
  nshift = cellID;
  
  C = ncoresetReg/(regBundleSize*interleaverSize);
  
  for r = 0:1:interleaverSize-1
    for c = 0:1:C-1
      j = c*interleaverSize + r;
      cceRegInterleaving(j+1,1) = mod(((r*C)+c+nshift), (ncoresetReg/regBundleSize));
    endfor
  endfor 
 endfunction 