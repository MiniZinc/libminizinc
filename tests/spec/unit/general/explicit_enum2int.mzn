/***
!Test
solvers: [chuffed]
options:
  -Werror: true
expected: !Result
  solution: !Solution
    x: 1
***/

enum Foo = {A};
int: x :: output = enum2int(A);
