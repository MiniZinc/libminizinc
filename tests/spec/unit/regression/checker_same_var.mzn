/***
!Test
solvers: [gecode]
extra_files:
- checker_same_var.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "x = 10;"
    objective: 5
***/

var 1..5: x;

solve maximize x;
