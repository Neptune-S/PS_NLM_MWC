function llr = ps_nr_qpsk_demodulation(eqDataRe,E)

llr     = [];
len_eqDataRe = E/2;

for i=1:len_eqDataRe
    llr = [llr;real(eqDataRe(i))];
    llr = [llr;imag(eqDataRe(i))];
end

end
