/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution
    x: -2147483646
    y: -2147483646
    z: -2147483646
- !Result
  solution: !Solution
    x: -500000000
    y: -500000000
    z: -500000000
***/

% Regression test for bug #67.  The version of mzn2fzn from 2009
% would abort because the wrong type-inst was inferred for the
% anonymous variable below.  This was (eventually) fixed in r13873.

var int: x;
var int: y;
var int: z;

constraint
    [x, y][_] = z;

solve :: int_search([x, y, z], input_order, indomain_min, complete) satisfy;
output [];