function eqRe =  ps_nr_pdcch_equalizer(dataRe,h)

    eqRe    =  dataRe.*conj(h); 
end