function llrOut  = ps_nr_pbch_de_ratematching(llrIn)
 
 P = [0,1,2,4,3,5,6,7,8,16,9,17,10,18,11,19,12,20,13,21,14,22,15,23,24,25,26,28,27,29,30,31]; 
 N = 512; 
 E = 864;
 llrOut = zeros(512,1);
 y = llrIn;
 j = 1;
 
 for i = N+1:1:E
    y(j,1) = (y(j,1) + llrIn(i,1))/2;   
    j = j + 1;
 end

% Sub-block De-interleaving %   
 fact = N/32;
 for n = 0:1:N-1    
   i = floor(32*n/N);
   j(n+1) = P(1,i+1) * fact + mod(n, fact);
 end
 
  llrOut(j(:) + 1, 1) = y(1:512, 1); 
  
 end