/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: "ambiguous overloading"
type: compile
***/

% Named arguments identify an overload by the names of the arguments a call supplies, not
% by their position. These two declarations give the same name to the same type, so the
% call `foo(x: a, y: b)` cannot tell them apart. Positional calls could still distinguish
% them, but the fully-named call must always resolve, so the pair is rejected outright.

predicate foo(var float: y, var int: x) = x <= y;
predicate foo(var int: x, var float: y) = x + y <= 10.0;

var 1..10: a;
var 1.0..10.0: b;
constraint foo(x: a, y: b);

solve satisfy;
