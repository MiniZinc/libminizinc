/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

int: x = 1 `` 2;
solve satisfy;
