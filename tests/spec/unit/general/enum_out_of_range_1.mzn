/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*parameter value out of range, declared range of `x' is {A}, but assigned value is B.*
***/

enum Foo = {A, B};
{A}: x = B;
