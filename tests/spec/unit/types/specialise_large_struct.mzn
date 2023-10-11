/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    t: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    u: [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
***/

function any $T: foo(any $T: x) = x;
tuple(
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1,
    var opt 1..1
): t :: output;
any: u :: output = foo(t);

constraint
    occurs(t.1) /\ occurs(t.2) /\ occurs(t.3) /\ occurs(t.4) /\
    occurs(t.5) /\ occurs(t.6) /\ occurs(t.7) /\ occurs(t.8) /\
    occurs(t.9) /\ occurs(t.10) /\ occurs(t.11) /\ occurs(t.12) /\
    occurs(t.13) /\ occurs(t.14) /\ occurs(t.15) /\ occurs(t.16);
