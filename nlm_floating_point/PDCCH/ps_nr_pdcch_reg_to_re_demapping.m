function [dataScReIdx, dmrsScReIdx] = ps_nr_pdcch_reg_to_re_demapping(rbCandPerAggr, coresetNysm, alIdx)

    L = [16 8 4];
    M = [1 2 4];

  numRegPerCcePerSym = 6/coresetNysm;
    
  pdcchDmrsScInd = [1 5 9] + 1;
  pdcchDataScInd = [0 2:4 6:8 10:11] + 1;

    dataScReIdx = zeros(L(alIdx)*numRegPerCcePerSym*coresetNysm*9,1);
    dmrsScReIdx = zeros(L(alIdx)*numRegPerCcePerSym*coresetNysm*3,1);
    aggStartRb = rbCandPerAggr;
    
      for rbIdx = 1:1:(L(alIdx)*numRegPerCcePerSym*coresetNysm)
        dataScReIdx((rbIdx-1)*9 + (1:9),1) = ((aggStartRb(rbIdx,1)-1)*12 + pdcchDataScInd).';
        dmrsScReIdx((rbIdx-1)*3 + (1:3),1) = ((aggStartRb(rbIdx,1)-1)*12 + pdcchDmrsScInd).';
      endfor
    
endfunction  