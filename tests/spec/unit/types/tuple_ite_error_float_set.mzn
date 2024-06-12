/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

var bool: b;
any: x = if b then ({1.5},) else ({2.5},) endif;
