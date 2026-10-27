/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_definition.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% The definition of t uses the array a as a whole, positively, so the elements of
% a are half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

array [1..2] of var bool: a = [foo(x, i) | i in 1..2];
var bool: t = exists(a);
constraint t \/ q;
