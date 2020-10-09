/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1.0
  - !Solution
    x: 2.0
  - !Solution
    x: null
***/

% Regression test for crash due to no opt float_dom definition

var opt {1.0, 2.0}: x :: add_to_output;

solve satisfy;