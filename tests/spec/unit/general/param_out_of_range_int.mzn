/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*parameter value out of range, declared range of `x' is -infinity\.\.-1 union 1\.\.infinity, but assigned value is 0.*
***/

set of int: Foo = -infinity..-1 union 1..infinity;
Foo: x = 0;
