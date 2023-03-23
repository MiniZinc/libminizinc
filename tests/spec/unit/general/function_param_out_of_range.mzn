/***
!Test
solvers:
- gecode
expected: !Error
  regex: '.*declared domain of `''<unnamed argument>'''' is 1\.\.1, but assigned value is 2.*'
***/

function int: foo({1}) = 1;
any: x = foo(2);
