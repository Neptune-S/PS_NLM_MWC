 function c  = ps_nr_pseudo_random_generator(cInit, seqLen) 
  
  goldSeqLen = 31;
  NC = 1600;
    
  x1 = zeros(1, (goldSeqLen + NC + seqLen));
  x2 = zeros(1, (goldSeqLen + NC + seqLen));
  
  x1(1) = 1;
  x2 = flip(dec2bin(cInit, 32) ~= '0');
  
  for i = 1:1: (NC  + seqLen)
    x1(i + goldSeqLen) = mod((x1(i) + x1(i + 3)), 2);
    x2(i + goldSeqLen) = mod((x2(i) + x2(i + 1) + x2(i + 2) + x2(i + 3)) ,2);
  end
  
  for i = 1:1:seqLen
   c(i,1) = mod((x1(NC + i) + x2(NC + i)), 2);  
  end
  
end