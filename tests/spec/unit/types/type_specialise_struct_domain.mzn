/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x:
      - a: [A]
      - a: [B]
      - a: [C]
***/

enum Foo = {A, B, C};
any: x :: output = arrayNd([A, B, C], [(a: [A]), (a: [B]), (a: [C])]);
