/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% Like var set of int, a var opt set of int needs a finite upper bound
var opt set of int: x;
