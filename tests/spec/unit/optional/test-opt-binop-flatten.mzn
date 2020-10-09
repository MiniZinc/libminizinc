/***
!Test
solvers: [gecode, chuffed, cbc]
expected: !Result
  solution: !Solution
    z: 5
  status: SATISFIED
***/

% Checks to ensure that correct evaluation is done when flattening vars that are actually par

var opt int: x = 10;
var opt int: y = 5;

var opt int: z :: add_to_output :: mzn_break_here = x - y;