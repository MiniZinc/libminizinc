/***
!Test
solvers: [gecode, chuffed, cbc]
expected: !Result
  solution: !Solution
    z: 10
  status: SATISFIED
***/

% Checks to ensure that bounds computation can deal with absent values
var opt int: x = 10;
var opt int: y = <>;

var opt int: z :: add_to_output :: mzn_break_here = x - y;