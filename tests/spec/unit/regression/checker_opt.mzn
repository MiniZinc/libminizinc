/***
!Test
solvers: [gecode]
extra_files:
- checker_opt.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "y = 20;"
***/

var 1..10: x;

solve maximize x;
