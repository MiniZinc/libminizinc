/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_mixed_use.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% a[1] is used positively, and in both polarities through xorall, so both
% elements are fully reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

array [1..2] of var bool: a = [foo(x, 1), foo(x, 2)];
constraint a[1] \/ q;
constraint xorall(a) \/ q;
