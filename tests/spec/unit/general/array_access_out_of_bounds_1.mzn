/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*array access out of bounds, array `x' has index set A\.\.B, but given index is C.*
***/

enum Foo = {A, B, C};
array [{A, B}] of int: x = [1, 2];
any: v = x[C];
