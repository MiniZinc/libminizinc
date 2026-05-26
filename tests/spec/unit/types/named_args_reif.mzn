/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_reif.fzn
***/

% Named-argument call in a reified context. The non-reif predicate is
% bodyless (a primitive); the user provides a hand-written _reif
% companion whose body the flattener should inline. The named-args
% disambiguation has to compose with reifyId so that the reif call
% gets bound to the matching _reif decl.

predicate p(var int: x, var int: y);
predicate p_reif(var int: x, var int: y, var bool: b) =
  b <-> (x + y = 7);

var 1..10: a;
var 1..10: bb;
var bool: r;

constraint r <-> p(x: a, y: bb);

solve satisfy;
