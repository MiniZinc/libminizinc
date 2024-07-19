/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc blackbox_multi_list.fzn
***/

% A propagator over an integer list, a float list and a fixed parameter. The
% integer list becomes the integer input channel, the float list the float
% input channel; the fixed parameter is not passed to the generic constraint.

include "experimental/blackbox.mzn";

array[1..2] of var 1..10: a;
array[1..2] of var 0.0..1.0: b;

predicate p(list of var int: xs, list of var float: ys, int: k) ::minizinc_value_propagator ::blackbox_exec("m.py");

constraint p(a, b, 7);

solve satisfy;
