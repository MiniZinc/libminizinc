/***
!Test
solvers: [chuffed, cbc]
expected: !Result
  solution: !Solution
    x: 1
    y: [!Range 1..3, !!set {}, !!set {1}]
    z: !Range 1..3
***/

% Tests for a bug in nosets.mzn for non-contiguous domains of x in array_var_set_element(x, y, z)

include "nosets.mzn";

var {1, 3}: x :: add_to_output;
array [1..3] of var set of 1..3: y :: add_to_output = [_, {}, {1}];
var set of 1..3: z :: add_to_output;

constraint card(z) = 3;
constraint array_var_set_element(x, y, z);