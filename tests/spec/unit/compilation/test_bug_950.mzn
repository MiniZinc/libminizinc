/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

function string: foo() = if false then "" elif false then "" else "" endif;

output [ f(i in 1..1) ([ foo() ]) ];
