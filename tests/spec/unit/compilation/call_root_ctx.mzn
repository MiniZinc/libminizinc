/***
!Test
type: compile
solvers: [gecode]
expected: !FlatZinc call_root_ctx.fzn
***/

% This used to produce a set_in due to the lets flattening with unknown partiality
function var int: foo() = let {
  var 1..2: bar = let { var 1..3: x } in x
} in bar;

var int: y :: add_to_output = foo();
