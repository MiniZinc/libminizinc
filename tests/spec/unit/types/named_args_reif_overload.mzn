/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_reif_overload.fzn
***/

% Two non-reif overloads with identical types and different parameter
% names, each paired with a corresponding _reif companion that shares
% the same parameter names. matchReifByNames propagates the named-args
% disambiguation through reifyId so each reified call binds to the
% matching _reif sibling and the correct body is inlined.

predicate interval(var int: start, var int: end) =
  start + end >= 0;
predicate interval_reif(var int: start, var int: end, var bool: b) =
  b <-> (start + end = 11);

predicate interval(var int: start, var int: duration) =
  start + duration >= 0;
predicate interval_reif(var int: start, var int: duration, var bool: b) =
  b <-> (start + duration = 99);

var 1..10: a;
var 1..10: bb;
var 1..200: c;
var bool: r;
var bool: s;

constraint r <-> interval(start: a, end: bb);
constraint s <-> interval(start: a, duration: c);

solve satisfy;
