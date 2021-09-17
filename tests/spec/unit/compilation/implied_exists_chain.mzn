/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc implied_exists_chain.fzn
***/

var bool: a;
var bool: b;
var bool: c;
var bool: r;

constraint r->forall(x in [a,b])(clause([x,c],[]));
