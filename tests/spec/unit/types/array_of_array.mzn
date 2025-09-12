/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    _output_item: !Trim |
      [[A, B, C], [D, E, F], [G, H, I]]
      [[{"e":"A"}, {"e":"B"}, {"e":"C"}], [{"e":"D"}, {"e":"E"}, {"e":"F"}], [{"e":"G"}, {"e":"H"}, {"e":"I"}]]
***/

enum Foo = {A, B, C, D, E, F, G, H, I};

test foo(array [int] of var $$E: x) = true;
test bar(array [int] of array [int] of var $$E: x) = true;

array [1..3] of array [1..3] of var Foo: x;
constraint foo(x[1]);
constraint bar(x);

include "globals.mzn";
constraint strictly_increasing(x[1] ++ x[2] ++ x[3]);

output [show(x), "\n", showJSON(x), "\n"];
