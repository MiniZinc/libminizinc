/***
!Test
solvers:
- gecode
expected: !Error
  regex: .*value for argument 1 out of bounds.*
***/

function int: foo({1}) = 1;
any: x = foo(2);
