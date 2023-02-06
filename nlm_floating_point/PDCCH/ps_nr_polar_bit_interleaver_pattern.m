function [interleaver_pattern] = ps_nr_polar_bit_interleaver_pattern(K,I_il)
  
% This pattern only depend upon input bit size and can be computed once for the DCI
% being searched. 38.212 Section 5.3.1.1
max_interlever_table = [0,2,4,7,9,14,19,20,24,25,26,28,31,34,42,45,49,50,51,53,54,56,58,59,61,62,65,66,67,69,70,71,72,76,77,81,82,83,87,88,89,91,93,95,98,101,104,106,108,110,111,113,115,118,119,120,122,123,126,127,129,132,134,138,139,140,1,3,5,8,10,15,21,27,29,32,35,43,46,52,55,57,60,63,68,73,78,84,90,92,94,96,99,102,105,107,109,112,114,116,121,124,128,130,133,135,141,6,11,16,22,30,33,36,44,47,64,74,79,85,97,100,103,117,125,131,136,142,12,17,23,37,48,75,80,86,137,143,13,18,38,144,39,145,40,146,41,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163];

K_il_max = 164;

interleaver_pattern = zeros(1,K);

n = 1:K;
m = 0:K_il_max-1;
 
if (I_il == 0)
  interleaver_pattern = n;
else
  temp = K_il_max-K;
  k = 1;
  for n = 1:K_il_max
     if(max_interlever_table(n)-temp>=0)
      interleaver_pattern(k) = max_interlever_table(n)-temp;
      k = k+1;
     end
  endfor  
endif  
  
end