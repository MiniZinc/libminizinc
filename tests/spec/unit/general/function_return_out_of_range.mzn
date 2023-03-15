/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*result of function `foo' is B, which violates function type-inst {A}.*
***/

enum Foo = {A, B};

function {A}: foo() = B;
any: x = foo();
