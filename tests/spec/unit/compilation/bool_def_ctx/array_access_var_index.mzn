/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc array_access_var_index.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% An access with a variable index in the root context uses the elements of the
% array positively. The element constraint takes the flat array in a mixed
% context, but that does not change the elements, which are already flattened,
% so they stay half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var 1..2: y;

constraint [foo(x, i) | i in 1..2][y];
