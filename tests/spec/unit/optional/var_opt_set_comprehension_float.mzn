/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% A variable where clause would produce var opt sets of float, which do not exist
array [1..2] of var bool: b;
any: c = [{1.0} | i in 1..2 where b[i]];
