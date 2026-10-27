/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc let_negated.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% A negated let: b1 and b2 are used negatively, so both are fully reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

constraint not (let { var bool: b1 = foo(x, 1); var bool: b2 = foo(x, 2) } in b1 \/ b2) \/ q;
