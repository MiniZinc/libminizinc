/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*function result violates function type-inst.*
***/

function tuple(1..2): foo() = (3,);
any: x =  foo();
