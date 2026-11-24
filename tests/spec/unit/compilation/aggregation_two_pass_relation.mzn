/***
!Test
solvers: [gecode]
options:
  -O2: true
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    a: 8
    x: [true, true]
  - !Solution
    a: 9
    x: [false, true]
  - !Solution
    a: 9
    x: [true, false]
  - !Solution
    a: 10
    x: [false, false]
***/

% The first pass names the relation c = a + sum(x) as a + sum(x) - c, and learns
% that it is at most 2. The second pass knows that c = 10, so the relation names
% a + sum(x). That variable must not take the bounds of the first one.

array [1..2] of var bool: x;
var 0..10: a;
var 0..10: c;
constraint c = a + sum(x);
constraint c = 10;
constraint (10 - c) div 4 <= (c - 10) div 3;

output ["a = \(a);\nx = \(x);\n"];
