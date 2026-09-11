% The constraint `int_global_cardinality_low_up_closed_reif`, as the FZnSO
% registry defines it.
%
% This file is a solver's override point: declare
% `int_global_cardinality_low_up_closed_reif` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

include "global_cardinality_low_up.mzn";

predicate fzn_int_global_cardinality_low_up_closed_reif(
  list of var int: xs,
  list of int: cover,
  list of int: lower_bound,
  list of int: upper_bound,
  var bool: b,
) =
  b <->
    (
      forall (i in index_set(xs)) (xs[i] in {d | d in array2set(cover)}) /\
        global_cardinality_low_up(xs, cover, lower_bound, upper_bound) /\
        % Implied condition
        length(xs) in sum(lower_bound)..sum(upper_bound)
    );
