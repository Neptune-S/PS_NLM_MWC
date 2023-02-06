function [dataRe,dmrsRe]= ps_nr_pbch_re_demapper(rxSymbols, cellId, nFFT, nCP)

nRePerRb     = 12;
nPBCHrb      = 20;
nPBCHsub     = nPBCHrb*nRePerRb; 
nSSBSymbol   = 4;
dmrsSpacing  = 4;
symbolSize   = nFFT+nCP;


fftGrid    = zeros(nFFT,nSSBSymbol);
dmrsRe     = [];
dataRe     = [];

for n = 1:nSSBSymbol
    fftGrid(:,n) = rxSymbols((n-1)*symbolSize+[(nCP+1):symbolSize]);
    fftGrid(:,n) = fft(fftGrid(:,n));
end    

v = mod(cellId,4);

% 2nd PBCH Symbol
subStartLow  = 392;
subStartHigh = 0;
rxSub        = [fftGrid(subStartLow+[1:nPBCHsub/2],2);fftGrid(subStartHigh+[1:nPBCHsub/2],2)];
nPBCHdrmsLoc = [0:dmrsSpacing:(nPBCHsub-dmrsSpacing)] + v + 1;
m = 1;
for k = 1:nPBCHsub
    if (m<=60) && (k == nPBCHdrmsLoc(m))  
        dmrsRe = [dmrsRe;rxSub(k)];
        m = m+1;
    else
        dataRe = [dataRe;rxSub(k)];
    end    
end    

% 3rd PBCH Symbol
subStartLow  = 392;
subStartHigh = 72;
nPBCHdrmsLoc = [0:dmrsSpacing:((2*nPBCHsub)/5-dmrsSpacing)] + v + 1;
rxSub        = [fftGrid(subStartLow+[1:nPBCHsub/5],3);fftGrid(subStartHigh+[1:nPBCHsub/5],3)];
m = 1;
for k = 1:2*(nPBCHsub/5)
    if (m<=24) && (k == nPBCHdrmsLoc(m))
        dmrsRe = [dmrsRe;rxSub(k)];
        m = m+1;
    else
        dataRe = [dataRe;rxSub(k)];
    end    
end  

% 4rd PBCH Symbol
subStartLow  = 392;
subStartHigh = 0;
rxSub        = [fftGrid(subStartLow+[1:nPBCHsub/2],4);fftGrid(subStartHigh+[1:nPBCHsub/2],4)];
nPBCHdrmsLoc = [0:dmrsSpacing:(nPBCHsub-dmrsSpacing)] + v + 1;
m = 1;
for k = 1:nPBCHsub
    if  (m<=60) && (k == nPBCHdrmsLoc(m))
        dmrsRe = [dmrsRe;rxSub(k)];
        m = m+1;
    else
        dataRe = [dataRe;rxSub(k)];
    end    
end  

end