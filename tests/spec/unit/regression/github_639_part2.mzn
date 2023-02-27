/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/
opt 1..1: abs = <>;
constraint absent(abs);

opt 1..3: prs = 1;
constraint occurs(prs);