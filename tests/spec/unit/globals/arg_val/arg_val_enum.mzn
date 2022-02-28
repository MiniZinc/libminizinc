/***
!Test
solvers: [gecode, chuffed]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    vals: [C, D]
    i: A
  - !Solution
    vals: [D, C]
    i: B
***/
include "all_different.mzn";
include "arg_val.mzn";

enum X = {A, B};
enum Y = {C, D};

array [X] of var Y: vals;
var X: i;

constraint i = arg_val(vals, C);
constraint all_different(vals);
