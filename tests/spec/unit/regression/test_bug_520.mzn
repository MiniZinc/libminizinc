/***
!Test
solvers: [gecode]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    _output_item: '{a}"b"'
***/

enum E = {a};
var set of E: x;
string: b = "b";
output [format(x), format(b)];
