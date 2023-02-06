function bitOut  = ps_nr_pbch_bit_deinterleaver(bitIn)
  
  pbchPayloadSize = 32;
  jsfn = 0;
  jhrf = 10;
  jssb = 11;
  jother = 14;

  pbchPayloadInterleaverPattern = [16,23,18,17,8,30,10,6,24,7,0,5,3,2,1,4,9,11,12,13,14,15,19,20,21,22,25,26,27,28,29,31];

  %pbchPayloadInterleaverPattern = [10,14,13,12,15,11,7,9,4,16,6,17,18,19,20,21,0,3,2,22,23,24,25,1,8,22,26,27,27,29,5,31];

  bitOut = [];
  
  % First 6 SFN Bits
  jsfn    = 0;
  jother  = 14;
  jhrf    = 10;
  jssb    = 11;

  k = 1;
  bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jother+1) + 1);
  k = k+1;
  jother = jother+1;
  
  for i = 1:1:6
      bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jsfn+1) + 1);
      k = k+1;
      jsfn = jsfn+1;
  end    
  
  for i = 1:1:17
      bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jother+1) + 1);
      k = k+1;
      jother = jother+1;
  end   

  for i = 1:1:4
      bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jsfn+1) + 1);
      k = k+1;
      jsfn = jsfn+1;
  end
  
  bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jhrf+1) + 1);
  k = k+1;
  
  for i = 1:1:3
      bitOut(k) = bitIn(pbchPayloadInterleaverPattern(jssb+1) + 1);
      k = k+1;
      jssb = jssb+1;
  end
 
end
