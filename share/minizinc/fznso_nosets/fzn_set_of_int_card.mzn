% `set_of_int_card` for a solver with no set variables: the set becomes the
% array of Booleans saying which members it holds, and the constraint is stated
% over that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_card(var set of int: s, var int: n) =
  let {
    array [int] of var bool: bx = set2bools(s);
  } in bool_lin_eq([1 | i in index_set(bx)], [bx[i] | i in index_set(bx)], n);
