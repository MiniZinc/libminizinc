/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc let_in_predicate.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% A let in a predicate that is called in a positive context: b1 and b2 are
% half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

predicate p(var int: z) = let { var bool: b1 = foo(z, 1); var bool: b2 = foo(z, 2) } in b1 \/ b2;

constraint p(x) \/ q;
