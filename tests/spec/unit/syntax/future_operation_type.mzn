/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

op(int: (int)): f;
solve satisfy;
