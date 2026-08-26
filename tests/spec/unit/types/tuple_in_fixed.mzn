/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    hit: true
    miss: false
***/

% Regression test: when every member of the tuple is fixed, the rewrite into a
% `table` constraint has no columns left, so the number of matching rows has to
% decide the constraint.

var 1..3: a = 1;
var 1..3: b = 2;

constraint (a, b) in [(1, 2)];

var bool: hit :: output = (a, b) in [(1, 3), (1, 2)];
var bool: miss :: output = (a, b) in [(1, 3)];
