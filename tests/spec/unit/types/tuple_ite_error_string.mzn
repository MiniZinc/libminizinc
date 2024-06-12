/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var bool: b;
any: x = if b then ("foo",) else ("bar",) endif;
