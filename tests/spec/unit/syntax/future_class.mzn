/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

class Foo(int: x);
solve satisfy;
