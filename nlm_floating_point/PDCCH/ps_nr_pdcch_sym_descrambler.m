function llrOut  = ps_nr_pdcch_sym_descrambler(llrIn, cellId, E)

if ((cellId > 1007) || (cellId < 0))
    disp("Invalid CellId");
end

Nid = cellId;   %since not using  a UE-specific search space
Nrnti = 0;

cInit    = mod((Nrnti*2^16+Nid),2^31);

% E = length of scrambled data
c = ps_nr_pseudo_random_generator(cInit, E);
c = 1 - 2.*c;

for i = 1:1:E
    llrOut(i,1) = llrIn(i) .* c(i,1);
end

end