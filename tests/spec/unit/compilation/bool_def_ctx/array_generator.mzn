/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_generator.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% The elements of an array are used through the variable of a generator over the
% array, not by the generator itself. In count, they are only used positively, so
% they are half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

array [1..2] of var bool: a = [foo(x, i) | i in 1..2];
constraint count (y in a) (y) >= 1 \/ q;
