/***
!Test
expected:
- !Result
  solution: !Solution
    execution_unit_1000_3: 0
    execution_unit_1000_4: 0
    uav1: 0
- !Result
  solution: !Solution
    execution_unit_1000_3: 1
    execution_unit_1000_4: 1
    uav1: 1
- !Result
  solution: !Solution
    execution_unit_1000_3: 2
    execution_unit_1000_4: 2
    uav1: 2
- !Result
  solution: !Solution
    execution_unit_1000_3: 3
    execution_unit_1000_4: 3
    uav1: 3
- !Result
  solution: !Solution
    execution_unit_1000_3: 4
    execution_unit_1000_4: 4
    uav1: 4
- !Result
  solution: !Solution
    execution_unit_1000_3: 5
    execution_unit_1000_4: 5
    uav1: 5
- !Result
  solution: !Solution
    execution_unit_1000_3: 6
    execution_unit_1000_4: 6
    uav1: 6
***/

% Regression test for bug #347: mzn2fzn's FlatZinc optimiser was incorrectly
% optimising away some of the equality constraints involving uav1 below.

include "alldifferent.mzn";                                                                                                                                               
var 0..6: uav1;
var 0..6: uav2;
var 0..6: uav3;
var 0..6: uav4;
var 0..6: execution_unit_1000_3;
var 0..6: execution_unit_1000_4;
constraint alldifferent([uav1, uav2, uav3, uav4]);
constraint execution_unit_1000_3 = uav1;
constraint execution_unit_1000_4 = uav1;
solve satisfy;
output [
    "uav1 = ", show(uav1), ";\n",
    "uav2 = ", show(uav2), ";\n",
    "uav3 = ", show(uav3), ";\n",
    "uav4 = ", show(uav4), ";\n"
];
