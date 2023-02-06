clear all;
close all;
clc;

fs              = 61.44e6;
% Receiver Processing
cellId          = 103;
n_slot          =  10;
% CoreSet 0 Parameter
      %      Index Pattern RBs Symbol offset
table_13_4 = [0  1 24 2 0;
              1  1 24 2 1;
              2  1 24 2 2;
              3  1 24 2 3;
              4  1 24 2 4;
              5  1 24 3 0;
              6  1 24 3 1;
              7  1 24 3 2;
              8  1 24 3 3;
              9  1 24 3 4;
              10 1 48 1 12;
              11 1 48 1 14;
              12 1 48 1 16;
              13 1 48 2 12;
              14 1 48 2 14;
              15 1 48 2 16];


% This should come from higher level;
mib_pdcch_high_msb = 15;
kssb               = 0;
coreset_start_sym  = 0;
rnti               = 65535;
crc_len            = 24;  
I_il               = 1;

agg_level_tbl	   = [16 8 4]';
num_cand_per_agg   = [1 2 4]';
coreset_rbs        = table_13_4(mib_pdcch_high_msb+1,3);
coreset_sym        = table_13_4(mib_pdcch_high_msb+1,4);
coreset_rb_offset  = table_13_4(mib_pdcch_high_msb+1,5);


load pdcch_txwaveform_NRB_48_Nsym_2_AL_16_Cand_1_CellId_103.mat; % Test Vector
offset             = n_slot*fs*0.5e-3;
tx_data = txWaveform(offset+(1:245800));

%tx_data = txWaveform; % Data is at 61.44 MSPS
tx_data_by_2 = tx_data(1:2:end); % Now at 30.72 MSPS

% ## Extract Coreset Symbols symbols->RBs in this order:

coreset_sym_rbs = ps_nr_pdcch_coreset_extraction(tx_data_by_2,coreset_sym,coreset_start_sym,coreset_rb_offset,coreset_rbs,kssb);

%scatter(real(coreset_sym_rbs),imag(coreset_sym_rbs));

NCCE = 6;
numDataScPerRB = 9;
numDmrsScPerRB = 3;
numCces = (coreset_rbs * coreset_sym)/NCCE;

if numCces >= 16
	aggr_start_idx = 1;
elseif(numCces >= 8)
	aggr_start_idx = 2;
else
	aggr_start_idx = 3;
end
numAggrLvl = 3;

cceRegInterleaving = ps_nr_pdcch_interleave_rb(coreset_rbs, coreset_sym, cellId);

for aggr_idx = aggr_start_idx:1:numAggrLvl
  
  [K, E, N]           = ps_nr_pdcch_calculate_K_N_E(agg_level_tbl(aggr_idx), coreset_rbs);
  info_bit_pattern    = ps_nr_polar_information_bit_pattern(K,E,N);
  interleaver_pattern = ps_nr_polar_bit_interleaver_pattern(K,I_il);
  
  
  numDataScPerAggr = agg_level_tbl(aggr_idx) * NCCE * numDataScPerRB;
  numDmrsScPerAggr = agg_level_tbl(aggr_idx) * NCCE * numDmrsScPerRB;

  pdcchDataSc = zeros(numDataScPerAggr,1);
  pdcchDmrsSc = zeros(numDmrsScPerAggr,1);

  for candIdx = 1:1:num_cand_per_agg(aggr_idx)

    disp(["Aggregation Level:" " " num2str(agg_level_tbl(aggr_idx)) " " "Candidate:" " " num2str(candIdx)])
    
    [pdcchDataSc, pdcchDmrsSc, dmrsScReIdx] = ps_nr_pdcch_cce_reg_demapping(coreset_sym_rbs, cellId, coreset_rbs, coreset_sym, agg_level_tbl(aggr_idx), candIdx, cceRegInterleaving);

    % DMRS Estimation and Extraction
    [dmrsRefRe]   =  ps_nr_pdcch_dmrs_gen(n_slot,coreset_start_sym,cellId,dmrsScReIdx,coreset_sym,numDmrsScPerAggr);

    % Channel Estimation
    [chEst]       =  ps_nr_pdcch_channel_estimation(pdcchDmrsSc,dmrsRefRe,numDmrsScPerAggr);

    % Equalization
    [eqDataRe]    =  ps_nr_pdcch_equalizer(pdcchDataSc,chEst);

    % Demodulation and LLR generation
    [llr]         = ps_nr_qpsk_demodulation(eqDataRe, E);

  	% Symbol level descrambling
 	  [llrDescram]  = ps_nr_pdcch_sym_descrambler(llr, cellId, E);

    % Symbol level rate recovery / rate de-matching (including sub-block deinterleaving)
    [llrDeRate]   = ps_nr_pdcch_de_ratematching(llrDescram, E, K, N);
    
    [dec_bits]    = ps_nr_polar_decoder(llrDeRate,interleaver_pattern,info_bit_pattern,N,E,K);
    
    [crc_check]   = ps_nr_pdcch_crc_check(dec_bits,rnti);
    
    if(crc_check == 1)
      disp("DCI Found !")
      dci_1_0_si_sib_1 = ps_nr_parse_dci_1_0_si_sib_1(dec_bits(1:(K-crc_len)))
      break;
    else
       disp("No DCI Found !")
    endif  
  endfor

endfor

