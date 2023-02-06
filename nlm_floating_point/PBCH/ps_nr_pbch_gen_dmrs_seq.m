function dmrsSeq = ps_nr_pbch_gen_dmrs_seq(iSsbBar, cellId )
  v = mod(cellId,4);
  cInit = 2048*(iSsbBar + 1)*(floor(cellId/4)+1)+64*(iSsbBar+1)+v;
  psSeq = (ps_nr_pseudo_random_generator(cInit,144*2));
  %modulation mapper
  dmrsSeq = (1/sqrt(2))*((1-2*psSeq(1:2:end))+1i.*(1-2*psSeq(2:2:end)));

  
  end 