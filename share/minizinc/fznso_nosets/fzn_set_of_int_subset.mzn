% `set_of_int_subset` for a solver with no set variables: the set becomes the
% array of Booleans saying which members it holds, and the constraint is stated
% over that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_subset(var set of int: s, var set of int: t) :: promise_total =
  let {
    array [int] of var bool: bx = set2bools(s);
    array [int] of var bool: by = set2bools(t);
  } in forall (i in index_set(bx)) (
    if not (i in index_set(by)) then not bx[i] else bx[i] -> by[i] endif
  );
