/***
!Test
solvers: [gecode]
options:
  -Werror: true
expected: !Error
  type: MiniZincError
***/

int: lambda = 3;
var 1..lambda: x;
solve satisfy;
