/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc let_mixed_use.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% b1 is used positively and negatively, so it is fully reified. b2 is only used
% positively, so it is half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

constraint let { var bool: b1 = foo(x, 1); var bool: b2 = foo(x, 2) } in (b1 \/ b2) /\ (not b1 \/ q);
