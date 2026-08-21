% The constraint `int_all_different_except`, as the FZnSO registry defines it.
%
% This file is a solver's override point: declare `int_all_different_except` and
% the body below is dropped in favour of the propagator, or ship a file of this
% name with a decomposition of your own.

include "global_cardinality_low_up.mzn";

predicate fzn_int_all_different_except(list of var int: xs, set of int: except) =
  %% if the variables in xs are bounded then use the gcc decomposition
  if forall (i in index_set(xs)) (has_bounds(xs[i])) then
    let {
      set of int: A = dom_array(xs) diff except;
    } in global_cardinality(xs, set2array(A), [0 | i in A], [1 | i in A])
  else
    %% otherwise use the neq decomposition
    forall (i, j in index_set(xs) where i < j) ((xs[i] in except /\ xs[j] in except) \/ xs[i] != xs[j])
  endif;
