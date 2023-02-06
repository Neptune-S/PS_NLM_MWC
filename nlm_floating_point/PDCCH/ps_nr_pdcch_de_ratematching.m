function llrOut  = ps_nr_pdcch_de_ratematching(descrambled, E, K, N)
 % ----------------------- BIT DE-INTERLEAVING --------------------
%Since I_BIL = 0; de-interleaved data is equivalent to descrambled data

%E = descrambled size

% ------------------------ BIT  DE-SELECTION ----------------------
y = zeros(N,1);

%repetition case

R = floor(E/N);
if E>=N
  for repeat = 1:R                        % LLRs corresponding to first N bits are averaged from repeated bits
    for k = 0:N-1
        y(k+1,1) = descrambled((repeat-1)*N + k+1,1)/R + y(k+1,1) ;
    end
  end

  for k = R*N:E-1
    y(k+1 - R*N,1) = (descrambled(k+1,1) + y(k+1 - R*N,1))/2 ;
  end


%punctured case
elseif K/E <= 7/16   %K/E = code rate
    for k = 0:E-1
        y(k+N-E+1,1) = descrambled(k+1,1);  % LLRs for the bits removed are set to zero
    end

%shortened case
else
    for k = 0:E-1
        y(k+1,1) = descrambled(k+1,1);
    end
    for k = E:N-1
        y(k+1,1) = 1024;                   % LLRs for the bits removed are set to a large value
    end
end

% ---------------------- SUB BLOCK DE-INTERLEAVING ------------------------
 P = [0,1,2,4,3,5,6,7,8,16,9,17,10,18,11,19,12,20,13,21,14,22,15,23,24,25,26,28,27,29,30,31];
 llrOut = zeros(N,1);
 J = zeros(N,1);

 fact = N/32;

 for n = 0:1:N-1
    i = floor(32*n/N);
    J(n+1) = P(1,i+1) * fact + mod(n, fact);
    llrOut(J(n+1)+1, 1) = y(n+1, 1);
 end


