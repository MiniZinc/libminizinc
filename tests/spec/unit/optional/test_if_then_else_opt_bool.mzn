/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    a: [true, false, true]
    x: null
  - !Solution
    a: [true, true, true]
    x: null
  - !Solution
    a: [false, false, true]
    x: false
  - !Solution
    a: [false, true, true]
    x: true
***/

array [1..3] of var bool: a :: add_to_output;

var opt bool: x :: add_to_output;

constraint a[3];
constraint fzn_if_then_else_opt_bool(a, [<>, true, false], x);