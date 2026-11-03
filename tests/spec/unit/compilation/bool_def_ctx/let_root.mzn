/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc let_root.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% A let in the root context: b1 and b2 are only used positively, so both are
% half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

constraint let { var bool: b1 = foo(x, 1); var bool: b2 = foo(x, 2) } in b1 \/ b2;
