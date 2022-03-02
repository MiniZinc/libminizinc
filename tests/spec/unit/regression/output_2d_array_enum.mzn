/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

enum Foo = {A};
array [1..1, 1..1] of var Foo: xxx;
output ["\(xxx)\n"];
