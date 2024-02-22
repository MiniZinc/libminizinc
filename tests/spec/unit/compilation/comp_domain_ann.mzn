/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc comp_domain_ann.fzn
***/

mzn_opt_annotate_computed_domains = true;

array[1..4] of var 1..4: x;
constraint sum(x) = product(x);
