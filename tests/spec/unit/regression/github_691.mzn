/***
!Test
solvers: [gecode]
expected:
  - !Result
    solution: !Solution
      a: -1
      c: -2
  - !Result
    solution: !Solution
      a: 0
      c: 0
  - !Result
    solution: !Solution
      a: 1
      c: 2
***/
var -100..100: a;
var -2..2: c ::output = 2 * a;
