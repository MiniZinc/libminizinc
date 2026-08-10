/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

tuple(int, int): (a, b) = (1, 2);
solve satisfy;
