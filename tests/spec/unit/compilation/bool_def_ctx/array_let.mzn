/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_let.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% The elements of a let array that is only used positively are half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

constraint let { array [1..2] of var bool: a = [foo(x, i) | i in 1..2] } in a[1] \/ a[2] \/ q;
