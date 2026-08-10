/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

int: x = 1
int: y = 2;
solve satisfy;
