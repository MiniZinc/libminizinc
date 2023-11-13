/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc clause_disable_hr.fzn
***/

% Disable half-reification of clause through compiler option
mzn_half_reify_clause = false;

var -10..10: x;
var bool: c;
var bool: d;

constraint c -> (x >= 0 /\ (d -> x <= 0));
