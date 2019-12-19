/***
!Test
solvers: [chuffed, cbc]
expected:
- !Result
  solution: !Solution
    x: 1
    y: [!!set {1}, !!set {2}, !!set {3}]
    z: !!set {1}
- !Result
  solution: !Solution
    x: 3
    y: [!!set {1}, !!set {2}, !!set {3}]
    z: !!set {3}
***/

% Tests for a bug in nosets.mzn for non-contiguous domains of x in array_set_element(x, y, z)

include "nosets.mzn";

var {1, 3}: x :: add_to_output;
array [1..3] of set of 1..3: y :: add_to_output = [{1}, {2}, {3}];
var set of 1..3: z :: add_to_output;

constraint array_set_element(x, y, z);