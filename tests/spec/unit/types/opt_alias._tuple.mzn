/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
type: compile
***/

type Foo = tuple(int);
opt Foo: x;
