/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x_to_y: !!set {A, B, C}
***/

enum Foo = {A, B, C};
var Foo: x;
var Foo: y;
var set of Foo: x_to_y :: output = x..y;
constraint card(x_to_y) = 3;
