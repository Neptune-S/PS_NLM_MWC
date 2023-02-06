function [extractedDmrs,ssbIndex] = ps_nr_pbch_dmrs_extraction(dmrsData,cellId) %dmrsData 1x144 and dmrsRefSeq 1x144 
  corrVal =  0;
  corrTemp= -1;
  ssbIndex=  0;
  v = mod(cellId,4) +1; %In C, v will start from 0
  
  for iSsbBar=0:7
        dmrsSeq=ps_nr_pbch_gen_dmrs_seq(iSsbBar, cellId );    
        corrVal=sum(conj(dmrsSeq).*dmrsData);
        if(real(corrVal) > corrTemp )
             corrTemp = real(corrVal); 
             ssbIndex = iSsbBar;
        end
        
        
  end  

 extractedDmrs = ps_nr_pbch_gen_dmrs_seq(ssbIndex, cellId ); 

end