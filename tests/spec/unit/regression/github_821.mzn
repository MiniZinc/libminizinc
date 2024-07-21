/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [B, A]
***/

enum E = {A, B};
array[1..2] of var E: x :: output = [
  if i == 1 then B else _ endif
  | i in 1..2
];

constraint x[1] != x[2];
