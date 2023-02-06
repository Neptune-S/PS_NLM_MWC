function [dmrsRefRe,ssbIndex] = ps_nr_pbch_ref_dmrs_estimation(dmrsRe,cellId) %dmrsData 1x144 and dmrsRefSeq 1x144
temp     = 0;
ssbIndex = 0;

for ssbIdx=0:7
    dmrsSeq = ps_nr_pbch_gen_dmrs_seq(ssbIdx, cellId);
    corrVal = sum(conj(dmrsSeq).*dmrsRe);
    if(abs(corrVal) > temp )
        temp     = abs(corrVal);
        ssbIndex = ssbIdx;
    end
end

    dmrsRefRe = ps_nr_pbch_gen_dmrs_seq(ssbIndex, cellId );
    
end