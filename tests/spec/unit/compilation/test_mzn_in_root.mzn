/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc test_mzn_in_root.fzn
***/

predicate pred(var int: x) =
  if false \/ mzn_in_root_context() then
    x = 3
  else
    x = 20
  endif;

var 0..10: x;
var 1..2: y;
var bool: r;

constraint pred(x);
constraint r->pred(y);
