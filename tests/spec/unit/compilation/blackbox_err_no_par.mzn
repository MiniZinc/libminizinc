/***
--- !Test
solvers: [gecode]
expected: !Error
  regex: .*requires a `par` overload.*
***/

% A functional value propagator requires a `par` overload of the propagation
% function, which is used to determine the length of its output.

include "experimental/blackbox.mzn";

array[1..2] of var 1..3: x;

function list of var int: g(list of var int: z) ::minizinc_value_propagator ::blackbox_dll("x");

constraint g(x)[1] = 2;

solve satisfy;
