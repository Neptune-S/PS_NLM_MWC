clear all
close all

load pbch_cell_id_102_ssb_idx_0_vector;
% Transmitter Parameters
%           DMRSTypeAPosition: 3
%                  CellBarred: 0
%        IntraFreqReselection: 0
%             PDCCHConfigSIB1: 4
%     SubcarrierSpacingCommon: 30

% Initializing MIB Structure
mib.systemFrameNumber        = 0;
mib.subCarrierSpacingCommon  = 0;
mib.ssb_subcarrierOffset     = 0;
mib.dmrs_typeA_Position      = 0;
mib.pdcch_configSIB1         = 0;
mib.cellBarred               = 0;
mib.intraFreqReselection     = 0;
mib.spare                    = 0;

% Receiver Processing
cellId          = 102;
Lmax            = 8; %Maximum SSB block in a burst
nFFT            = 512;
ssbIdx          = 1; % 1 to 8. Mind not starting from zero
ssbStartSymbols = [2 8 16 22 30 36 44 50]; % -1 for the octave indexing
ssbSymbols      = 4;
ssbStartSymbol  = ssbStartSymbols(ssbIdx)+1; % Change this for each SSB
cpLengths       = [44,36,36,36,36,36,36,36,36,36,36,36,36,36,44,36,36,36,36,36,36,36,36,36,36,36,36,36];
cpSize          = cpLengths(ssbStartSymbol);
symbolLengths   = [556,548,548,548,548,548,548,548,548,548,548,548,548,548,556,548,548,548,548,548,548,548,548,548,548,548,548,548];
symbolLength    = symbolLengths(ssbStartSymbol);

% Extract SSB Symbols:
rxSSBsymbols =  rxWaveform(sum(symbolLengths(1:(ssbStartSymbol-1)))+[1:symbolLength*ssbSymbols]);

% Demapping data and dmrs re's
[dataRe,dmrsRe]= ps_nr_pbch_re_demapper(rxSSBsymbols, cellId, nFFT, cpSize);

% DMRS Estimation and Extraction
[dmrsRefRe,ssbIndex] = ps_nr_pbch_ref_dmrs_estimation(dmrsRe,cellId);

% Channel Estimation
[chEst] = ps_nr_pbch_channel_estimation(dmrsRe, dmrsRefRe);

% Equalization
[eqDataRe] = ps_nr_pbch_equalizer(dataRe,chEst,1);

% Demodulation and LLR generation
[llr] = ps_nr_pbch_demodulation(eqDataRe);

% Symbol level descrambling
[llrDescram]  = ps_nr_pbch_sym_descrambler(llr, ssbIndex,cellId);

% Symbol level De ratematching
[llrDeRate]   = ps_nr_pbch_de_ratematching(llrDescram);

% Polar Decoding
[crcFlag,decBits] = ps_nr_pbch_polar_decoder(llrDeRate);

if(crcFlag == 1)
    
    display('CRC Pass!')
    
    [bitDescram]  = ps_nr_pbch_bit_descrambling(decBits, cellId, 8);
    [pbchPaylod]  = ps_nr_pbch_bit_deinterleaver(bitDescram);
    
    % Mib Parsing
    
    mib.systemFrameNumber = sum(2.^[5:-1:0].*pbchPaylod(2:7));

    if(pbchPaylod(8) == 1)
        mib.subCarrierSpacingCommon = 'scs30';
    else
        mib.subCarrierSpacingCommon = 'scs15';
    end

    mib.ssb_subcarrierOffset = sum(2.^[3:-1:3].*pbchPaylod(9:12));

    if(pbchPaylod(13) == 1)
        mib.dmrs_typeA_Position = 'pos3';
    else
        mib.dmrs_typeA_Position = 'pos2';
    end
  
    mib.pdcch_configSIB1 = sum(2.^[7:-1:0].*pbchPaylod(14:21));

    mib.cellBarred       = pbchPaylod(22);

    mib.intraFreqReselection = pbchPaylod(23);

    mib.spare = pbchPaylod(24);

    mib.sfn = sum(2.^[0:9].*[pbchPaylod(7:-1:2) pbchPaylod(28:-1:25)]);

    mib.half_frame_bit =  pbchPaylod(29);

    mib.k_ssb_msb = pbchPaylod(30);
end


mib

