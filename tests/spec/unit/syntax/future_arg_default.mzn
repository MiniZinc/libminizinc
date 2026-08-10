/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

int: y = foo(int: x = 3);
solve satisfy;
