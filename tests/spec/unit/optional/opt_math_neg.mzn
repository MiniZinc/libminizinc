/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !Solution
***/

var opt -1..1: x;
var opt int: neg_x :: output = -x;

output let {
    constraint assert(
        fix(x) + fix(neg_x) = 0 /\ fix(x) = -fix(neg_x),
        "incorrect value for -x (x = \(x), -x = \(neg_x))"
    );
} in "Ok";
