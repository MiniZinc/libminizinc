/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel output_make_arg_par.ozn
***/

% Test to ensure that the signature for foo in the ozn is correct (all par)

var 1..3: x;
var 1..3: y;
function var int: foo(var int: v) = v * x;
any: z :: output = foo(y);
solve minimize x + y;
