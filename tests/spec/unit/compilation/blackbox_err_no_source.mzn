/***
--- !Test
solvers: [gecode]
expected: !Error
  regex: .*MiniZinc-defined black-box propagators are not yet supported.*
***/

% A propagator kind annotation without a `blackbox_exec`/`blackbox_dll` source
% is rejected: MiniZinc-defined propagator evaluation is not yet implemented.

include "experimental/blackbox.mzn";

array[1..3] of var 1..10: x;

predicate my_check(list of var int: xs) ::minizinc_value_propagator;

constraint my_check(x);

solve satisfy;
