/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    b: false
    y: 0
  - !Solution
    b: true
    y: 1
***/

var bool: b;
var int: x = bool2int(b);
var 0..1: y;

constraint y <= x;
constraint x <= y;
