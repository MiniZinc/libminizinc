/***
--- !Test
type: compile
solvers: [chuffed]
expected: !FlatZinc blackbox_bounds_default.fzn
***/

% A bounds propagator without a companion `_reason` function: the compiler
% inserts the conservative default reason (all input bounds explain every
% output bound).

include "experimental/blackbox.mzn";

array[1..2] of var 1..10: x;

predicate foo(list of var int: xs) ::minizinc_bounds_propagator ::blackbox_exec("bounds.py");

constraint foo(x);

solve satisfy;
