/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    c: [true]
  - !Solution
    x: 2
    c: [false]
***/

function tuple(var bool): foo(var int: x) = (x = 1,);
var 1..2: x;
any: c :: output = foo(x);
