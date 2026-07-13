/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

% See opt_struct_var_field_tuple.mzn.
opt record(var int: x): r;
solve satisfy;
