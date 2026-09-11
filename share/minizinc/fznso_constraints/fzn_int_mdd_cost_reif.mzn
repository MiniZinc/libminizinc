% The constraint `int_mdd_cost_reif`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_mdd_cost_reif` and the
% body below is dropped in favour of the propagator, or ship a file of this name
% with a decomposition of your own.

predicate fzn_int_mdd_cost_reif(
  list of var int: xs,
  int: nodes,
  list of int: level,
  int: edges,
  list of int: from,
  list of set of int: label,
  list of int: weight,
  list of int: to,
  var int: total_cost,
  var bool: b,
) =
  abort("Reified cost_mdd/9 is not supported.");
