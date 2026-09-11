% `set_of_int_ne` for a solver with no set variables: the set becomes the array
% of Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_ne(var set of int: s, var set of int: t) :: promise_total =
  let {
    array [int] of var bool: bx = set2bools(s);
    array [int] of var bool: by = set2bools(t);
  } in exists (i in index_set(bx) union index_set(by)) (
    if not (i in index_set(bx)) then
      by[i]
    elseif not (i in index_set(by)) then
      bx[i]
    else
      bx[i] != by[i]
    endif
  );
