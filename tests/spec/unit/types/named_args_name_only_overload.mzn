/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_name_only_overload.fzn
***/

% Overloads with identical parameter types and different parameter names
% (the §7.1 "killer case"). The named-argument syntax
% selects the intended overload; matchFn(Call*)'s param-name filter
% keeps the selection stable through flatten-time re-resolution so the
% right predicate body is inlined into FlatZinc.

predicate interval(var int: start, var int: end) =
  start + end = 11;
predicate interval(var int: start, var int: duration) =
  start + duration = 99;

var 1..10: a;
var 1..10: b;
var 1..200: c;

constraint interval(start: a, end: b);
constraint interval(start: a, duration: c);

solve satisfy;
