% The constraint `set_of_int_seq_precede_chain_reif`, as the FZnSO registry
% defines it.
%
% This file is a solver's override point: declare
% `set_of_int_seq_precede_chain_reif` and the body below is dropped in favour of
% the propagator, or ship a file of this name with a decomposition of your own.

predicate fzn_set_of_int_seq_precede_chain_reif(
  list of var set of int: xs,
  var bool: b,
) =
  abort("Reified seq_precede_chain constraint is not supported");
