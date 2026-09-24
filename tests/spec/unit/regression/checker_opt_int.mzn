/***
!Test
solvers: [gecode]
extra_files:
- checker_opt_int.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "CORRECT"
***/

% Optional variables are passed to the checker (an undefined opt parameter in the
% checker must not be silently initialised to <>)
var opt 1..3: x;
var opt 1..3: y;
constraint x = 2;
constraint absent(y);
