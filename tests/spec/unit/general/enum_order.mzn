/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

enum Foo;
Foo = {A, B} ++ {C} ++ F(1..3) ++ {D};
enum Bar = {W, X} ++ {Y} ++ G(1..3) ++ {Z};
constraint assert(set2array(Foo) = [A, B, C, F(1), F(2), F(3), D], "Wrong value for Foo");
constraint assert(set2array(Bar) = [W, X, Y, G(1), G(2), G(3), Z], "Wrong value for Bar");
