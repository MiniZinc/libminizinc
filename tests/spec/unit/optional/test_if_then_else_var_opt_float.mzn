/***
!Test
solvers: [gecode]
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
    x: 2.4
  - !Solution
    a: [false, true, true]
    x: 1.2
***/

array [1..3] of var bool: a :: add_to_output;
array [1..3] of var opt {1.2, 2.4}: b :: add_to_output;

var opt {1.2, 2.4}: x :: add_to_output;

constraint a[3];
constraint fzn_if_then_else_var_opt_float(a, b, x);

constraint absent(b[1]) /\ occurs(b[2]) /\ occurs(b[3]);
constraint b[2] < b[3];