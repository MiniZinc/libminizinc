/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*declared domain of `x' is -infinity\.\.-1.0 union 1.0\.\.infinity, but assigned value is 0\.0.*
***/

set of float: Foo = -infinity..-1.0 union 1.0..infinity;
Foo: x = 0.0;
