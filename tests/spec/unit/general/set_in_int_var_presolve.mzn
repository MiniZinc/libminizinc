/***
!Test
solvers: [gecode]
options:
  -O3: true
  --only-range-domains: true
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 1
  - !Solution
    x: 5
***/

% Test for GitHub issue #1027, where the Gecode presolver posted a set_in
% constraint on an integer variable as if it were a Boolean variable.
var int: x;
constraint x in {1, 3, 5};
constraint x != 3;
solve satisfy;
