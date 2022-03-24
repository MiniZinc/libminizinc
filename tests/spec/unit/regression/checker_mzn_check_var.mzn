/***
!Test
solvers: [gecode]
extra_files:
- checker_mzn_check_var.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "Ok"
***/

% Previously the mzn_check_var annotation for y went missing from the output
% and so was not passed to the checker

predicate foo() = y = 1;
var bool: x :: output = foo();
var 1..2: y;
