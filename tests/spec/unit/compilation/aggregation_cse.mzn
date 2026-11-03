/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_cse.fzn
***/

% Expect one int_lin_le_imp shared by both clauses because canonical term order
% gives the reordered sums the same CSE key.

var 0..10: x;
var 0..10: y;
var 0..10: z;
var bool: q;
var bool: r;

constraint (x + y + z >= 3) \/ q;
constraint (z + y + x >= 3) \/ r;
