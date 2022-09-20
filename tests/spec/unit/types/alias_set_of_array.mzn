/***
!Test
solvers: [gecode]
expected:
- !Error
  type: TypeError
type: compile
***/
type Foo = array [int] of int;
type Bar = set of Foo;
Bar: x;
