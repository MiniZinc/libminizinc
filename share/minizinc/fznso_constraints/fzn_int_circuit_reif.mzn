% The constraint `int_circuit_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_circuit_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

include "all_different.mzn";

predicate fzn_int_circuit_reif(list of var int: xs, int: offset, var bool: b) =
  abort("Reified circuit/2 is not supported.");
