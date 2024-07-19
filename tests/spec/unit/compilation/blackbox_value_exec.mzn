/***
--- !Test
type: compile
solvers: [chuffed]
expected: !FlatZinc blackbox_value_exec.fzn
***/

% A relational value propagator implemented by an external executable. The
% compiler generates a body that calls the generic `blackbox` constraint with a
% single boolean output (fixed to true by the constraint context).

include "experimental/blackbox.mzn";

array[1..3] of var 1..10: x;

predicate my_check(list of var int: xs) ::minizinc_value_propagator ::blackbox_exec("checker.py");

constraint my_check(x);

solve satisfy;
