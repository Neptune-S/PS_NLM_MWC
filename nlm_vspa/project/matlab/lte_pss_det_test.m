clear all;
clear classes;
% close all;
% clc;

% Add paths
addpath('../../../common/matlab/utils');
addpath('../../../common/matlab/vspa');
addpath('../../matlab');
addpath('../../../../vspalibrary/core_library');
addpath('../../../matlab_toolbox');

% Get testplan
lte_pss_det_testplan;

global LTE;

band = '1.4MHz';
mode = 'FDD';
LTE_Config(band,mode);

% Make in/ref directories
if ~isdir(TC_IN_PATH)
    mkdir(TC_IN_PATH);
end
if ~isdir(TC_REF_PATH)
    mkdir(TC_REF_PATH);
end

nCrit = 0;
nFalseDet = 0;
nMisDetP = 0;
nMisDetS = 0;
freqIdx = [34:64 66:96];
cfoHz = 0;
rssiThresh = 128*2^-20;
snrdB = -7;
nBitADCloss = 2;
timeOffset = 0;
nMisDet = 0;
step = 128;
N = 256;
scsHz = 15000;
freqHypo = [-22500 -15000 -7500 0 7500 15000 22500] / LTE.SAMPLE_RATE;   %list of normalized FO candidates
% freqHypo = [-21000 -14000 -7000 0 7000 14000 21000] / LTE.SAMPLE_RATE;   %list of normalized FO candidates

nFreqHypo = length(freqHypo);
detPss = struct('N', 256, ...
                'L', 128, ...
                'M', 129, ...
                'vspPrec', 'hfix', ...    % 'sp' or 'hfix'
                'th', 0.13);
detSss = struct('sssPwrFd' , 1, ...
                'thre', 0.13, ...
                'prec', 'sp');
 
for id2 = 1:3            
    pssFreq(:,id2) = fftshift([zeros(33,1); LTE.PSS(1:end/2,id2); 0; LTE.PSS(end/2+1:end,id2); zeros(32,1)]);
end
pssTime = ifft(pssFreq,N/2,1) * sqrt(128);

detPss.dtHFix = r_half( pssTime / max(max([abs(real(pssTime)); abs(imag(pssTime))])) ); 

% detPss.tdPwr = sum(abs(pssTime(:,1)).^2)/length(pssTime);            
detPss.tdPwr = 1;%62/128;

%% Generate eNB signals
config = lteTestModel('3.2', '10MHz');
config.TotSubframes = 10; 
config.DuplexMode = mode;
config.PDCCHPower = 0;
config.NCellID = 0;
[inp, ~, ~] = lteTestModelTool(config);
upsampFact = 614400 / length(inp);
inpRes = resample(inp,upsampFact,1);

config = lteTestModel('3.2', '10MHz');
config.TotSubframes = 10; 
config.DuplexMode = mode;
config.PDCCHPower = 0;
config.NCellID = 4;
[inp2, ~, ~] = lteTestModelTool(config);
upsampFact = 614400 / length(inp2);
inp2 = circshift(inp2, [16 0]); 
inp2 = inp2 .* exp(1i* 2*pi * 100/LTE.SAMPLE_RATE * [0:length(inp2)-1].');
inpRes2 = resample(inp2,upsampFact,1);
scale2 = mean(abs(inpRes).^2) / mean(abs(inpRes2).^2);
inpRes2 = inpRes2 * sqrt(scale2);

config = lteTestModel('3.2', '10MHz');
config.NCellID = 8;
[inp3, ~, ~] = lteTestModelTool(config);
upsampFact = 614400 / length(inp3);
inp3 = circshift(inp3, [-16 0]); 
inp3 = inp3 .* exp(1i* 2*pi * (-200)/LTE.SAMPLE_RATE * [0:length(inp3)-1].');
inpRes3 = resample(inp3,upsampFact,1);
scale3 = mean(abs(inpRes).^2) / mean(abs(inpRes3).^2);
inpRes3 = inpRes3 * sqrt(scale3);

config = lteTestModel('3.2', '20MHz');
config.NCellID = 9;
[inp4, ~, ~] = lteTestModelTool(config);
upsampFact = 614400 / length(inp4);
inp4 = circshift(inp4, [-16 0]); 
inp4 = inp4 .* exp(1i* 2*pi * (-100)/LTE.SAMPLE_RATE * [0:length(inp4)-1].');
inpRes4 = resample(inp4,upsampFact,1);
scale4 = mean(abs(inpRes).^2) / mean(abs(inpRes4).^2);
inpRes4 = inpRes4 * sqrt(scale4);

sigPwr = mean(abs(inpRes).^2);

%% Generate testvectors
for tcInd = 1 : length(TC_ARRAY)   
    % Current testcase name
    tcName = TC_ARRAY(tcInd).tcName;
    fprintf('-> Generating testcase %s ...\n', tcName);
    
    % Random generator seed for testvector reproducibility
    rng('default'); rng(63);
% rng(tcInd);

    % Testcase directories
    tcInDir  = [TC_IN_PATH,  tcName, '/'];
    tcOutDir = [TC_OUT_PATH, tcName, '/'];
    tcRefDir = [TC_REF_PATH, tcName, '/'];
    
    % Create directories
    if ~isdir(tcInDir)
        mkdir(tcInDir);
    end
    if ~isdir(tcRefDir)
        mkdir(tcRefDir);
    end
    
    % Testcase parameters
    size  = TC_ARRAY(tcInd).size;
    
    % Generate random input
    
    sigPwr = mean(abs(inpRes).^2);
    noiseSeq   = 1/sqrt(2)*(randn(length(inpRes),1)+1i*randn(length(inpRes),1))* sqrt(sigPwr / (10^(snrdB/10)));
    noisePwr = mean(abs(noiseSeq).^2);
        
    rxData = inpRes + noiseSeq;
%     rxData = rxData + inpRes2/sqrt(1.6) + inpRes3/sqrt(1.6) + inpRes4/sqrt(1.6);
    rxData = rxData / ( max([abs(real(rxData(:))); abs(imag(rxData(:)))]) + 15/2^15);      % scaling
    rxData = rxData / 2^nBitADCloss;
    rxData = rxData .* exp(1i* 2*pi * cfoHz/61.44e6 * [0:length(rxData)-1].');
    rxData = circshift(rxData, [length(rxData)/2 0]);
    rxData = rxData(22529-4096:end);
    
    nFiltStages = 5;
    nCoeffs = [8 8 8 8 8];
    filtDelays = [];
    flt = struct();
    data = r_half(rxData);
    normCut = [0.27 0.27 0.27 0.27 0.27];
    for m = 1:nFiltStages
        [flt] = lte_decim2_coeff_get( nCoeffs(m), normCut(m) );
        filtDelays = [filtDelays; flt.delay];
        flt.rhf = [0.015625000000000 0.108551025000000 0.202819820000000 0.261962900000000 0.261962900000000 0.202819820000000 0.108551025000000 0.015625000000000];
        [data, out_m, ~] = r_decim2x_padd(flt.rhf, [zeros(length(flt.rhf)-1,1); data], 'HF');
        data = r_half(data);
    end
    fltDel = round(sum(filtDelays ./ (2.^[nFiltStages:-1:1]')));

    
    inpDelay = circshift(data, [timeOffset 0]);   % comment this line and uncomment previous line to bypass decimation
    testSig = inpDelay .* exp(1i* 2*pi * cfoHz/LTE.SAMPLE_RATE * [0:length(data)-1].');
    testSig = testSig(1:LTE.SAMPLES_PER_SLOT*4);
    testSig = r_half(testSig);
 
            
    out_vspa = r_lte_pss_det(testSig, detPss, detSss, rssiThresh);
    % ---------------------- Export input data ----------------------------
    % Export input
    filePath = [tcInDir, 'inp.bin'];
    vspaExportComplex([rxData(1:1+3*4096-1); zeros(32,1)], 'half_fixed', filePath);
% 
%     % Export control structure
%     filePath = [tcInDir, 'ctrl.bin']; 
%     out_len = length(out_vspa)/2;
%     ctrl_data = [size, out_len];
%     vspaExportReal(ctrl_data, 'uint', filePath);
%     
%     % --------------------- Export reference data -------------------------  
%     % Export reference output
%     filePath = [tcRefDir, 'out.bin'];
%     vspaExportReal(out_vspa, 'half', filePath);
    
end

fprintf('\n Testcase generation done ...\n\n\n');
