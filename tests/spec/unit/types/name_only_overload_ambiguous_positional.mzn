/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "call to `foo' is ambiguous"
type: compile
***/

% "Name-only" overloads (declarations that differ only in a parameter name) can
% only be told apart by named arguments. A purely positional call therefore does
% not determine which body to run, so it must be a type error rather than
% silently resolving to whichever sibling is registered first.

predicate foo(var int: upper) = upper >= 0;
predicate foo(var int: lower) = lower <= 10;

var 0..5: x;
constraint foo(x);

solve satisfy;
