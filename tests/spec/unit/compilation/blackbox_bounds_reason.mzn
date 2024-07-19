/***
--- !Test
type: compile
solvers: [chuffed]
expected: !FlatZinc blackbox_bounds_reason.fzn
***/

% A bounds propagator with a user-defined `foo_reason` function; the compiler
% inserts a call to it as the reason argument.

include "experimental/blackbox.mzn";

array[1..2] of var 1..10: x;

predicate foo(list of var int: xs) ::minizinc_bounds_propagator ::blackbox_dll("b.so");
function list of tuple(int, list of tuple(int, PropBnd), list of tuple(int, PropBnd)): foo_reason(list of var int: xs) =
  [ (i, [(i, PR_LB)], [(i, PR_UB)]) | i in index_set(xs) ];

constraint foo(x);

solve satisfy;
