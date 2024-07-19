/***
--- !Test
type: compile
solvers: [chuffed]
expected: !FlatZinc blackbox_value_fn_dll.fzn
***/

% A functional value propagator implemented by a shared library. The output
% array's index set is derived from the `par` overload of the propagation
% function, evaluated on the lower bounds of the input variables.

include "experimental/blackbox.mzn";

array[1..3] of var 1..10: x;

function list of int: bar(list of int: xs) = [ xs[i] + 1 | i in index_set(xs) ];
function list of var int: bar(list of var int: xs) ::minizinc_value_propagator ::blackbox_dll("bb.so");

constraint bar(x)[1] = 5;

solve satisfy;
