/***
!Test
solvers: [gecode]
extra_files:
- checker_params.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "yay!"
***/

% Ensure that parameters are correctly added to the output model when required by the solution checker

var 1..2: x;
int: y :: output_only = fix(x) + 1;

solve maximize x;
