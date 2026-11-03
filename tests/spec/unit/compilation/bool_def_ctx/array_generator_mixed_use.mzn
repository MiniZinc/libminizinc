/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_generator_mixed_use.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% The elements of the array are used in both polarities through the variable of
% a generator over the array, so they are fully reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

array [1..2] of var bool: a = [foo(x, i) | i in 1..2];
constraint sum (y in a) (bool2int(y)) = 1;
