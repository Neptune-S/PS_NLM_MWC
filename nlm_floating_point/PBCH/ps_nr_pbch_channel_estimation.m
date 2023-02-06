%{
Interpolation Explaination 
Case v=0;    
              d1 d1 d1    d2 d2 d2    d3 d3 d3   --> Interpolated LS Estimate per RB
           __ __ __ __ __ __ __ __ __ __ __ __ 
           d1          d2          d3
Case v=1;    
           d1    d1 d1 d2    d2 d2 d3    d3 d3   --> Interpolated LS Estimate Per RB
           __ __ __ __ __ __ __ __ __ __ __ __ 
              d1          d2          d3
Case v=2;    
           d1 d1    d1 d2 d2    d2 d3 d3    d3   --> Interpolated LS Estimate Per RB
           __ __ __ __ __ __ __ __ __ __ __ __ 
                 d1          d2          d3
Case v=3;    
           d1 d1 d1    d2 d2 d2    d3 d3 d3      --> Interpolated LS Estimate Per RB
           __ __ __ __ __ __ __ __ __ __ __ __ 
                    d1          d2          d3
                    
 So, Just by repeating three consecutive set of LS Estimate 3 times will conver 
 9 RE data location present in 1 RB. 
 Considering Coherence Bandwidth of 7 RE.  
%}
function h = ps_nr_pbch_channel_estimation(dmrsData, dmrsRefSeq) %dmrsData 1x144 and dmrsRefSeq 1x144
    
    %LS Estimate
    lsEstimate =  dmrsData.*conj(dmrsRefSeq);

    %Channel Interpolation ignoring edges
    h          = repelem(lsEstimate,3);

end