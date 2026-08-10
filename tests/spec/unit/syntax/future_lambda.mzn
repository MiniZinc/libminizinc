/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

any: f = lambda int: (int: x) => x + 1;
solve satisfy;
