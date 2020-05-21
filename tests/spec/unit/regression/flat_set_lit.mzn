/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution:
  - !Solution
    x: !set {2}
***/

% Regression test for GitHub bug #392
% Previously {a} would not be correctly evaluated during flattening

var set of 1..3: foo(var 1..3: a) = {a};
var set of 1..3: x :: add_to_output = foo(2);
