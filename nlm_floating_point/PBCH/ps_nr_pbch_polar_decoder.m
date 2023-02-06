
function [crcFlag outBits] = ps_nr_pbch_polar_decoder(llrIn)

%% SC decoder
    
E    = 864;
N    = 512;
K    = 56;
P    = 24;
A    = K-P;
n    = log2(N);
L    = zeros(n+1,N);
ns   = zeros(1,2*N-1); 
bits = zeros(n+1,N);

infoBitPattern = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1];

crcInterleaverPattern = [1,3,4,6,8,11,12,13,15,16,19,20,22,25,27,31,32,33,2,5,7,9,14,17,21,23,26,28,34,10,18,24,29,35,30,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56];

%Initialsing beliefs   
L_val(1, :) = llrIn; 

node  = 0; 
depth = 0; 
done  = 0;            

while (done == 0)
  %leaf or not
  if depth == n
    if (infoBitPattern(node + 1) == 0) %is node frozen
      bits(n + 1, node + 1) = 0;
    else
      if L_val(n + 1, node + 1) >= 0
        bits(n + 1, node + 1) = 0;
      else
        bits(n + 1, node + 1) = 1;
      end
    end
    if node == (N - 1)
      done = 1;
    else
      node = floor(node / 2);
      depth = depth - 1;
    end
  else
    % nonleaf
    % position of node in node state vector
    npos = (2 ^ depth - 1) + node + 1;
    if ns(npos) == 0
      temp = 2 ^ (n - depth);
      % incoming belief
      Ln = L_val(depth + 1, temp * node + 1 : temp * (node + 1));
      % split beliefs into 2
      a = Ln(1 : temp / 2);
      b = Ln(temp / 2 + 1 : end);
      % next node: left child
      node = node * 2;
      depth = depth + 1;
      % incoming belief length for left child
      temp = temp / 2;
      % minsum and storage
      L_val(depth + 1, temp * node + 1 : temp * (node + 1)) = f_min_sum(a, b);
      ns(npos) = 1;
    else
      if ns(npos) == 1 %step R and go to right child
        temp = 2 ^ (n - depth);
        %incoming beliefs
        Ln = L_val(depth + 1, temp * node + 1 : temp * (node + 1));
        %split beliefs into 2
        a = Ln(1 : temp / 2); b = Ln(temp / 2 + 1 : end);
        %left child
        lnode = 2 * node;
        ldepth = depth + 1;
        ltemp = temp / 2;
        %incoming decisions from left child
        bitsn = bits(ldepth + 1, ltemp * lnode + 1 : ltemp * (lnode + 1));
        %next node: right child
        node = node *2 + 1;
        depth = depth + 1;
        %incoming belief length for right child
        temp = temp / 2;
        %g and storage
        L_val(depth + 1, temp * node + 1 : temp * (node + 1)) = g_reptition(a, b, bitsn);
        ns(npos) = 2;
     else %step U and go to parent
        temp = 2 ^ (n - depth);
        %left and right child
        lnode = 2 * node;
        rnode = 2 * node + 1;
        cdepth = depth + 1;
        ctemp = temp/2;
        %incoming decisions from left child
        bitsl = bits(cdepth + 1, ctemp * lnode + 1 : ctemp * (lnode + 1));
        %incoming decisions from right child
        bitsr = bits(cdepth + 1, ctemp * rnode + 1 : ctemp * (rnode + 1));
        %combine
        bits(depth + 1, temp * node + 1 : temp * (node + 1)) = [mod(bitsl + bitsr, 2) bitsr];
        node = floor(node / 2);
        depth = depth - 1;
      end
     end
  end
end

dataBits = bits(n+1,find(infoBitPattern));

dataBitsInterleaved = zeros(1,K);

dataBitsInterleaved(crcInterleaverPattern) = dataBits;

crc     = ps_nr_pbch_crc(dataBitsInterleaved(1:A));

crcFlag = ~(length(find(dataBitsInterleaved(A+1:end)-crc))>0);

outBits = dataBitsInterleaved(1:end-P);

end

function out = f_min_sum(a, b) 
  out = (1 - 2 * (a < 0)) .* (1 - 2 * (b < 0)).* min(abs(a), abs(b));
end

function out = g_reptition(a,b,c)
  out = b + (1 - 2 * c) .* a;
end  