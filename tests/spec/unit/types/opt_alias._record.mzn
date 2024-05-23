/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

type Foo = record(int: a);
opt Foo: x;
