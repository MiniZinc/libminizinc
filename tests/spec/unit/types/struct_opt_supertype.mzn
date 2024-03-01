/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
***/

enum Foo = { A };
array [int] of tuple(opt Foo): person = [(A,), (<>,)];
