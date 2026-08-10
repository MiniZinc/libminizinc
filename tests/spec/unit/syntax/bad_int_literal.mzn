/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

int: x = 99999999999999999999999;
solve satisfy;
