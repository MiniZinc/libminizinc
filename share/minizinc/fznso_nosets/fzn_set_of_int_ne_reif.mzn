% `set_of_int_ne_reif` for a solver with no set variables: the set becomes the
% array of Booleans saying which members it holds, and the constraint is stated
% over that. Shadows the abort in `fznso_constraints/`.

include "fzn_set_of_int_eq_reif.mzn";
include "fznso_set_to_bools.mzn";

predicate fzn_set_of_int_ne_reif(var set of int: s, var set of int: t, var bool: b) :: promise_total =
  fzn_set_of_int_eq_reif(s, t, not b);
