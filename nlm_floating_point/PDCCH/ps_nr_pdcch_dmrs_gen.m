function pdcchDMRS = ps_nr_pdcch_dmrs_gen(NSlot,nsym,nID,dmrsScReIdx,coreset_sym,numDmrsScPerAggr)
    
    prbInd       = floor(dmrsScReIdx/12)+1;
    prbInd       = prbInd(1:3:end);
    len_prbInd   = numDmrsScPerAggr/3;
    prbInd       = reshape(prbInd,[len_prbInd/coreset_sym , coreset_sym]);
    dmrsSym      = complex(zeros(3*size(prbInd,1), size(prbInd,2)));
    
    for i_sym=1:coreset_sym

        numPRB       = max(prbInd(:,i_sym));
        cinit        = mod(2^17*(14*NSlot +nsym+1)*(2*nID+1)+2*nID,2^31); 
        n            = 6*numPRB; 
        cSeq         = ps_nr_pseudo_random_generator(cinit,n);            
        cSeqRB       = reshape(cSeq,6,[]);% 6=3*2 , 3REs per RB, 2 bits per DM-RS symbol
        cSequence    = cSeqRB(:,prbInd(:,1));

        % QPSK modulation mapper
        dmrsSym(:,i_sym) = (1/sqrt(2))*((1-2*cSequence(1:2:end))+1i.*(1-2*cSequence(2:2:end)));
        nsym         = nsym+1;

    end
    
    pdcchDMRS = dmrsSym(:);
end