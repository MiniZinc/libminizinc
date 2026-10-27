/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc toplevel.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% A top-level variable that is only used positively is half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

var bool: t = foo(x, 1) \/ foo(x, 2);
constraint t \/ q;
