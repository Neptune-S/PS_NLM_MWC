function [crcPassFlag]  = ps_nr_pdcch_crc_check(bitsIn, rnti) 
 
 crcPassFlag  = 0; 
 K = size(bitsIn,2);
 L = 24;
 A = K - L;
 rntiLen = 16;
# rnti = 65535;
 
 rxCrc =  bitsIn(1, end-L+1:end);
 aDash = zeros(1,K);
 aDash(1,1:L) = 1;
 aDash(1, L+1:K) = bitsIn(1,1:A);
 crcPoly = [1 1 0 1 1 0 0 1 0 1 0 1 1 0 0 0 1 0 0 0 1 0 1 1 1 ]; #24C
 
 i = 1;
 input = [aDash zeros(1,L)];
 dataShift = [0 input(1, 1:L)];
 
 while(i <= K)
 bitOut = [dataShift(1, 2:end) input(1, i+L)];
 dataShift = bitOut;
 
 if(bitOut(1,1) == 1)
      dataShift = xor(crcPoly, bitOut);  
 endif 
 
 i = i + 1;
 endwhile
 
 calCrc = dataShift(1, 2:end);
 x2 = flip(dec2bin(rnti, 16) ~= '0');
 calCrc(1,end-rntiLen+1:end) = mod((calCrc(1,end-rntiLen+1:end)  + x2),2);

if(calCrc == rxCrc)
 crcPassFlag  = 1;
else
 crcPassFlag  = 0;
endif

endfunction