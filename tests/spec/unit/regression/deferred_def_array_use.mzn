/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 2
    y: false
  - !Solution
    x: 2
    y: true
***/

% The definition of t uses the array a as a whole, and is flattened after its
% elements. a[1] is used positively, but also through t in xorall, so it must
% not be half-reified: else x = 3 and x = 4 are wrong solutions.

var 0..4: x;
var bool: y;
var bool: q;
array [1..2] of var bool: a = [x > 1, x > 2];
var bool: t = xorall(a);
constraint a[1] \/ y;
constraint t \/ q;
constraint not q;

output ["x = \(x);\ny = \(y);\n"];
