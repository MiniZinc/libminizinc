/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

% See opt_struct_var_tuple.mzn.
var opt record(int: x): r;
solve satisfy;
