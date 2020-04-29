/***
!Test
type: output-model
solvers: [gecode]
expected: !OutputModel output_only_fn.ozn
***/

% Previously the declaration for foo would not get copied to the output model

function int: foo() = 10;
int: x :: output_only = foo();
output [show(x)];