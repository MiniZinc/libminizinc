/***
!Test
solvers: [gecode]
expected: !Error
  regex: '.*arg_val: the value does not occur in the array.*'
***/
% arg_val on a value that does not occur in a par array of strings is undefined
include "arg_val.mzn";

array [1..2] of string: ids = ["a", "b"];
int: i = arg_val(ids, "c");
