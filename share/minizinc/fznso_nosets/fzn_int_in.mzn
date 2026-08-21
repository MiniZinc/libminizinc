% `int_in` for a solver with no set variables: the set becomes the array of
% Booleans saying which members it holds, and the constraint is stated over
% that. Shadows the abort in `fznso_constraints/`.

include "fznso_set_to_bools.mzn";

predicate fzn_int_in(int: x, var set of int: values) :: promise_total =
  let { array [int] of var bool: by = set2bools(values) } in by[x];

predicate fzn_int_in(var int: x, var set of int: values) :: promise_total =
  let { array [int] of var bool: by = set2bools(values) } in by[x];
