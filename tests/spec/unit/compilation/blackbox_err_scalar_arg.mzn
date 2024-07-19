/***
--- !Test
solvers: [gecode]
expected: !Error
  regex: .*black-box propagator variable arguments must be.*
***/

% A black-box propagator may only take `list of var int` / `list of var float`
% decision-variable arguments, not a scalar `var int`.

include "experimental/blackbox.mzn";

var 1..10: y;

predicate q(var int: z) ::minizinc_value_propagator ::blackbox_exec("x");

constraint q(y);

solve satisfy;
