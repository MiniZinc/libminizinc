/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [!!set {A}, !!set {B}, !!set {C}]
***/

enum Foo = {A, B, C};
array [1..3] of var set of Foo: x;
var set of Foo: x_union = array_union(x);
var set of Foo: x_intersection = array_intersect(x);

constraint forall (x_i in x) (card(x_i) = 1);
constraint x_union = Foo;
constraint x_intersection = {};
constraint forall (i in 1..2) (min(x[i]) < min(x[i + 1]));
