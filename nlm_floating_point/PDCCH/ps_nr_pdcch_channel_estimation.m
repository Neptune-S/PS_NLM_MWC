%{
Interpolation Explaination 

    
           d1    d1 d1 d2    d2 d2 d3    d3 d3   --> Interpolated LS Estimate Per RB
           __ __ __ __ __ __ __ __ __ __ __ __ 
              d1          d2          d3

                    
 So, Just by repeating three consecutive set of LS Estimate 3 times will conver 
 9 RE data location present in 1 RB. 
%}

function h = ps_nr_pdcch_channel_estimation(dmrsData,dmrsRefSeq,nDmrsRe)

    %LS Estimate
    lsEstimate =  dmrsData.*conj(dmrsRefSeq);

    %Channel Interpolation ignoring edges
    h          = repelem(lsEstimate,3);
    
end