/***
!Test
extra_files:
- github_776.dzn
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !AnonEnum
      enumName: A
      value: 1
***/
enum A;
var A: x;
