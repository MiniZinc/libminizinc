/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: '[[{"e":"A"}, {"e":"B"}], [{"e":"C"}, {"e":"D"}]] [[{"e":"A"}, {"e":"B"}], [{"e":"C"}, {"e":"D"}]]'
***/

include "globals.mzn";
enum Foo = {A, B, C, D};
array [1..2, 1..2] of var Foo: x;
constraint strictly_increasing(array1d(x));
output [showJSON([|A, B | C, D|]), " ", showJSON(x)];
