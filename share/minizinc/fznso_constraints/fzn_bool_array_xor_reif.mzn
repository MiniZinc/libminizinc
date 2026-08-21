% The constraint `bool_array_xor_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `bool_array_xor_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_bool_array_xor_reif(list of var bool: xs, var bool: b) =
  b <-> (sum (i in index_set(xs)) (bool2int(xs[i])) mod 2 = 1);
