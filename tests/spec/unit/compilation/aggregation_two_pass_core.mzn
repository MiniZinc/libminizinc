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
    a: 9
    x: [false, false]
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

% The sum in the definition of y shares the core a + sum(x) - c with the second
% constraint in the first pass, which bounds it above by 0. In the second pass,
% c = 10 moves into the constant, so the core is a + sum(x). That variable must
% not take the bounds of the first one.

array [1..2] of var bool: x;
var 0..10: a;
var 0..10: c;
var -100..100: y;
constraint y = (2 * a + 2 * sum(x) - 2 * c + 2) div 3;
constraint a + sum(x) - c <= 0;
constraint c = 10;
constraint a >= 9;

output ["a = \(a);\nx = \(x);\n"];
