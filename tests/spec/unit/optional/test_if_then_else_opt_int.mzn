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
    x: 2
  - !Solution
    a: [false, true, true]
    x: 1
***/

array [1..3] of var bool: a :: add_to_output;

var opt 1..2: x :: add_to_output;

constraint a[3];
constraint fzn_if_then_else_opt_int(a, [<>, 1, 2], x);