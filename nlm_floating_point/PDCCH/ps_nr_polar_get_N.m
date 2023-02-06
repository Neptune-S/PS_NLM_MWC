function [N] = ps_nr_polar_get_N(E,K,n_max)

% Finding the value of n

if((E<=(9/8)*2^(ceil(log2(E)-1)))&&((K/E)<(9/16)))
  n1 = ceil(log2(E))-1;
else
  n1 = ceil(log2(E));
endif

R_min = 1/8;
n2   = ceil(log2(K/R_min));
n_min = 5;
n     = max([min([n1 n2 n_max]) n_min]); 
N     = 2^n;

end  