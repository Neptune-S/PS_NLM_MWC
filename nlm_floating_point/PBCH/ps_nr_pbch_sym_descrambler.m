function llrOut  = ps_nr_pbch_sym_descrambler(llrIn, ssbIndex, cellId)

validSSBIdx = [0 : 7];

if ((cellId > 1007) || (cellId < 0))
    disp("Invalid CellId");
end

if (~any(validSSBIdx == ssbIndex))
    disp("Invalid SSB Index");
end

maxSSB   = 8;
seqLen   = 864;
pnLength = maxSSB * seqLen;
cInit    = cellId;

c = ps_nr_pseudo_random_generator(cInit, pnLength);
c = 1 - 2.*c;

for i = 1:1:seqLen
    llrOut(i,1) = llrIn(i) .* c(i + (ssbIndex * seqLen),1);
end

end