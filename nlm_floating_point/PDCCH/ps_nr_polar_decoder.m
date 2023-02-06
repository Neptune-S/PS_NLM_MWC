function [outBits] = ps_nr_polar_decoder(llrIn,interleaver_pattern,info_bit_pattern,N,E,K)

n        = log2(N);
L        = zeros(n+1,N);
ns       = zeros(1,2*N-1); 
bits     = zeros(n+1,N);

%Initialsing beliefs   
L_val(1, :) = llrIn; 

node  = 0; 
depth = 0; 
done  = 0;            

while (done == 0)
  %leaf or not
  if depth == n
    if (info_bit_pattern(node + 1) == 0) %is node frozen
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

dataBits = bits(n+1,find(info_bit_pattern));
outBits  = zeros(1,K);
outBits(interleaver_pattern+1) = dataBits;


function out = f_min_sum(a, b) 
  out = (1 - 2 * (a < 0)) .* (1 - 2 * (b < 0)).* min(abs(a), abs(b));
end

function out = g_reptition(a,b,c)
  out = b + (1 - 2 * c) .* a;
end  

end


