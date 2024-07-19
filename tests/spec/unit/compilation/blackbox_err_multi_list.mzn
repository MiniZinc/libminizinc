/***
--- !Test
solvers: [gecode]
expected: !Error
  regex: .*at most one `list of var int'.*
***/

% A black-box propagator may take at most one `list of var int` (and at most one
% `list of var float`); a second integer list is rejected.

include "experimental/blackbox.mzn";

array[1..2] of var 1..5: a;
array[1..2] of var 1..5: b;

predicate p(list of var int: xs, list of var int: ys) ::minizinc_value_propagator ::blackbox_exec("x");

constraint p(a, b);

solve satisfy;
