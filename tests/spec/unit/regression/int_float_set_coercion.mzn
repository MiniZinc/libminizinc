/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    cmp: [true, true, false, true, false, true, false, true, false, true, true]
    un: !!set {1.0, 2.5}
    un2: !!set {1.0, 2.5}
    q: !!set {1.0, 2.0, 3.0}
    x: 2.0
***/

% A set of int used where a set of float is expected is coerced element-wise (1..3 becomes
% {1.0, 2.0, 3.0}). Mixing them used to fail with "not a bool expression", unify $T to a
% non-set type, coerce the set into an array, or crash.
set of float: b = {1.0, 2.5};
function set of $T: g(set of $T: x, set of $T: y) = x union y;
array [int] of bool: cmp :: output = [
  {1} subset b, b superset {1}, {1} = b, b != {1}, 1..2 subset b, {1} < b, 1.5 in {1, 2},
  1.0 in 1..2, 1..2 = 1.0..2.0, 1..2 = {1.0, 2.0}, 1 in b,
];
set of float: un :: output = {1} union b;
set of float: un2 :: output = g({1}, b);
set of float: q :: output = 1..3;
var float: x :: output;
constraint x in 1..3 /\ x > 1.2 /\ x < 2.8;
