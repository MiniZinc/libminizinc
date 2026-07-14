include "global_cardinality_low_up.mzn";
predicate fzn_alldifferent_except(array [int] of var int: xs, set of int: except) =
  %% if the variables in xs are bounded then use the gcc decomposition
  if forall (i in index_set(xs)) (has_bounds(xs[i])) then
    let {
      set of int: A = dom_array(xs) diff except;
    } in global_cardinality(xs, set2array(A), [0 | i in A], [1 | i in A])
  else
    %% otherwise use the neq decomposition
    forall (i, j in index_set(xs) where i < j) ((xs[i] in except /\ xs[j] in except) \/ xs[i] != xs[j])
  endif;
