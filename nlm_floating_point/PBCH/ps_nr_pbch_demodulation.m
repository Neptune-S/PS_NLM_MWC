
function llr = ps_nr_pbch_demodulation(eqDataRe)

llr     = [];
nDataRe = 432;

for i=1:nDataRe
    llr = [llr;real(eqDataRe(i))];
    llr = [llr;imag(eqDataRe(i))];
end

end