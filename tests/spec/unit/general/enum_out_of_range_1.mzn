/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*declared domain of `x' is {A}, but assigned value is B.*
***/

enum Foo = {A, B};
{A}: x = B;
