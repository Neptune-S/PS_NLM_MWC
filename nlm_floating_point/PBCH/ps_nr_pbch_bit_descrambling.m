function bitOut  = ps_nr_pbch_bit_descrambling(bitIn, cellId, numSsbBlocks)

validNumSSB = [4 8 64];
bitOut = [];

if ((cellId > 1007) || (cellId < 0))
    disp("Invalid CellId");
end

if (~any(validNumSSB == numSsbBlocks))
    disp("Invalid Number of SSB");

end

pbchPayloadInterleaverPattern = [16,23,18,17,8,30,10,6,24,7,0,5,3,2,1,4,9,11,12,13,14,15,19,20,21,22,25,26,27,28,29,31];
pbchPayloadSize = 32;

halfIdxBitPos =  pbchPayloadInterleaverPattern(1, 10 + 1);
sfnLsb2BitPos =  pbchPayloadInterleaverPattern(1, 8 + 1);
sfnLsb3BitPos =  pbchPayloadInterleaverPattern(1, 7 + 1);

if(numSsbBlocks == 64)
    M = pbchPayloadSize - 6;
else
    M = pbchPayloadSize - 3;
end

pnLength = 4 * M;
cInit    = cellId;
c = ps_nr_pseudo_random_generator(cInit, pnLength);
v = 2*bitIn(sfnLsb3BitPos + 1) + bitIn(sfnLsb2BitPos + 1);

j = 1;
for i = 1:1:pbchPayloadSize
    if((i == halfIdxBitPos + 1) || (i == sfnLsb2BitPos + 1) || (i == sfnLsb3BitPos + 1))
        bitOut(i, 1) = bitIn(i);
    else
        bitOut(i, 1) = mod((bitIn(i) + c(j + v*M)), 2);
        j = j + 1;
    end
end

end
