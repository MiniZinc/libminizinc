/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*function result violates function type-inst, array contains value B which is not contained in {A}.*
***/

enum Foo = {A, B};

function array [int] of opt {A}: foo() = [B, <>];
any: x = foo();
