/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

annotation foo;
annotation bar;
var bool: b;
any: x = if b then (foo,) else (bar,) endif;
