/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 1
    y: 2
  - !Solution
    x: 2
    y: 2
  - !Solution
    x: 1
    y: 3
  - !Solution
    x: 2
    y: 3
***/


function tuple(var int): bar(var int: x) = let {
  var dom(x): v;
} in (v,);

predicate foo(var int: x) = let {
  tuple(var int): d = bar(x);
} in x = d.1;

var 1..2: x;
var 2..3: y;

constraint foo(x);
constraint foo(y);
