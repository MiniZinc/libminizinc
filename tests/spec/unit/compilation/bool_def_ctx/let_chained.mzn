/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc let_chained.fzn
***/

% Boolean definitions are flattened in the context of their uses.
% b1 is used positively through the later let variable b3, so it is
% half-reified.

predicate foo(var int: x, int: i);
predicate foo_reif(var int: x, int: i, var bool: r);
predicate foo_imp(var int: x, int: i, var bool: r);

var 0..10: x;
var bool: q;

constraint let {
  var bool: b1 = foo(x, 1);
  var bool: b3 = b1 \/ q;
  var bool: b2 = foo(x, 2);
} in b3 \/ b2;
