
function crc = ps_nr_pbch_crc(inBits)
    
crcPoly  = [1,1,0,1,1,0,0,1,0,1,0,1,1,0,0,0,1,0,0,0,1,0,1,1,1];
crcLen   = 24;

crcRem   = zeros(1,crcLen+1);
tmpArray = [inBits, zeros(1,crcLen)];

% Loop to calculate CRC bits
for(n=1:length(inBits)+crcLen)
    for(m=1:crcLen)
        crcRem(m) = crcRem(m+1);
    end
    crcRem(crcLen+1) = tmpArray(n);

    if(crcRem(1) ~= 0)
        for(m=1:crcLen+1)
            crcRem(m) = mod(crcRem(m)+crcPoly(m), 2);
        end
    end
end

for(n=1:crcLen)
    crc(n) = crcRem(n+1);
end