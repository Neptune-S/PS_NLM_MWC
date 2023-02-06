function eqRe =ps_nr_pbch_equalizer(dataRe,h,N)

eqRe = dataRe.*conj(h);

end