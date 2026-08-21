% `int_in_reif` for a solver with no set variables: the set becomes the array of
% Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_int_in_reif(int: x, var set of int: values, var bool: b) :: promise_total =
  if x in ub(values) then b <-> set2bools(values)[x] else not b endif;

predicate fzn_int_in_reif(var int: x, var set of int: values, var bool: b) :: promise_total =
  b <-> set2bools(values)[x];
