/***
!Test
solvers: [gecode]
extra_files:
- checker_var_opt_set.mzc.mzn
expected:
- !Result
  solution: !Solution
    _checker: !Trim "CORRECT"
***/

var opt set of 1..3: x;
var opt set of 1..3: y;
constraint occurs(x) /\ card(deopt(x)) = 2 /\ not (2 in deopt(x));
constraint absent(y);
