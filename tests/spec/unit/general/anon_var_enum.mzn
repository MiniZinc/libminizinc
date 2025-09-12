/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [A, B]
***/

enum Foo = {A, B};
any: x :: output = [A, _];
constraint x[1] != x[2];
