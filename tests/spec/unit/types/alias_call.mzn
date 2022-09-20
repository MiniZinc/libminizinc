/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 1
***/

type Foo = trace("Foo\n", 1..3);
var Foo: x;
constraint x < 2;
