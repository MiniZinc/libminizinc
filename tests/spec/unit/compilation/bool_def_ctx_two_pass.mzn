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
    x: 1
    y: 0
  - !Solution
    x: 2
    y: 0
  - !Solution
    x: 2
    y: 1
  - !Solution
    x: 2
    y: 2
***/

% Boolean definitions that are flattened after their uses, in the second pass of
% -O2: b0 and b1 are fixed by the first pass, and the definitions of g for i = 1
% and i = 2 must not share their variables. The elements of a are flattened after
% the uses of a as well.

var 0..2: x;
var 0..2: y;
var bool: b0 = x > 5;
var bool: b1 = x > 0;
constraint b1;
constraint b0 \/ b1;
constraint forall (i in 1..2) (let { var bool: g = x + i > y } in g);
array [1..2] of var bool: a = [x > 1, y > 5];
constraint exists(a) \/ y = 0;

output ["x = \(x);\ny = \(y);\n"];
