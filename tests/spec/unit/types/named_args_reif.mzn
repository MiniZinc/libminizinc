/***
!Test
solvers: [gecode]
type: compile
expected: !FlatZinc named_args_reif.fzn
***/

% Named-argument call in a reified context. `p` is a user-defined (bodied)
% predicate, so named arguments are available for it; a body-less primitive
% would be positional-only and the named call rejected. The named-args
% disambiguation has to compose with reification so that the reified call
% flattens to the body of the selected overload.

predicate p(var int: x, var int: y) = (x + y = 7);

var 1..10: a;
var 1..10: bb;
var bool: r;

constraint r <-> p(x: a, y: bb);

solve satisfy;
