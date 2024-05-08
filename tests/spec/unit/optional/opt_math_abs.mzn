/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !Solution
***/

var opt -1..1: x;
var opt int: abs_x :: output = abs(x);

output let {
    constraint assert(
        fix(abs_x) >= 0 /\ (fix(x) = fix(abs_x) \/ fix(x) = -fix(abs_x)) /\ fix(abs_x) = abs(fix(x)),
        "incorrect value for abs(x) (x = \(x), abs(x) = \(abs_x))"
    );
} in "Ok";
