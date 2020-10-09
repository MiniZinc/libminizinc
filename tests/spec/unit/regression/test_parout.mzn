/***
--- !Test
solvers: [gecode, chuffed]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    x: 10
    y: -2147483646
- !Result
  status: SATISFIED
  solution: !Solution
    x: 10
    y: 11
--- !Test
solvers: [cbc]
expected: !Error {}
***/

int: X = 10;
var int: x :: add_to_output = X;
var int: y :: add_to_output;
constraint x != y;
solve satisfy;
output ["\(x) \(y)\n"];
