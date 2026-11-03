/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc toplevel_mixed_use.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% t1 is used positively, and negatively through t2, so it is fully reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

var bool: t1 = foo(x, 1);
var bool: t2 = not t1 \/ q;
constraint t2 \/ t1;
