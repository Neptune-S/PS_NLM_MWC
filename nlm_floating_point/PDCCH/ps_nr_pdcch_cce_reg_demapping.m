function [pdcchDataSc, pdcchDmrsSc, dmrsScReIdx] = ps_nr_pdcch_cce_reg_demapping(coreset_sym_rbs, cellID, coresetNRB, coresetNysm, aggrLvl, candIdx, cceRegInterleaving)
  
  numCCE = (coresetNRB*coresetNysm)/6;  
  L = [16 8 4];
  M = [1 2 4];
  
  if(aggrLvl == 16)
    alIdx = 1;
  endif
  
 if(aggrLvl == 8)
    alIdx = 2;
  endif
  
  if(aggrLvl == 4)
    alIdx = 3;
  endif

  Y = 0;
  nCI = 0;
  #numCCE candidates per aggregation Level ##
  m = candIdx-1;
  for i = 0:1:L(alIdx)-1
      pddcCandidates(i+1,1) = L(alIdx)*(mod((Y + floor((m*numCCE)/(L(alIdx)*M(alIdx))) + nCI), floor(numCCE/L(alIdx)))) + i;
  endfor
  
  numRegPerCcePerSym = 6/coresetNysm;   
   
   # Find starting reg  for each Candidate
    startRegCandPerAgg = zeros(L(alIdx), 1);
     #all candidates 
    startRegCandPerAgg(:,1) = cceRegInterleaving(pddcCandidates(:,1) + 1) * numRegPerCcePerSym + 1;
  
      rbCandPerAggr = [];
      regCandPerAgg = zeros(L(alIdx)*numRegPerCcePerSym*coresetNysm,1);
      
      i = 1;
      for symIdx = 1:1: coresetNysm     
        for intlIdx = 1:1:L(alIdx)
        regCandPerAgg(i, 1) = startRegCandPerAgg(intlIdx, 1) + ((symIdx-1)*coresetNRB);
           for cceIdx = 1:1:numRegPerCcePerSym-1
            i = i +1;
            regCandPerAgg(i, 1) = regCandPerAgg(i-1, 1) + 1;
           endfor
           i = i+1;
        endfor 
      endfor
     rbCandPerAggr = sort(regCandPerAgg);
  
  [dataScReIdx, dmrsScReIdx] = ps_nr_pdcch_reg_to_re_demapping(rbCandPerAggr, coresetNysm,alIdx);
   pdcchDataSc = coreset_sym_rbs(dataScReIdx);
   pdcchDmrsSc = coreset_sym_rbs(dmrsScReIdx);
   
endfunction