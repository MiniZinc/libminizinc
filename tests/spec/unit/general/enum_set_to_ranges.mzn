/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: [P, Q, S, S]
***/

enum A = {P, Q, R, S};
any: x :: output = set_to_ranges({P, Q, S});
