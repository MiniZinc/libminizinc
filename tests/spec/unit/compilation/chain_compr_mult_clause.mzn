/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc chain_compr_mult_clause.fzn
***/
var bool: a;
var bool: b;
var bool: c;
var bool: r;

constraint r -> (clause([a],[b]) /\ clause([a],[c]));
