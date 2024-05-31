/***
!Test
solvers: [gecode]
options:
  keep-paths: true
expected: !Result
  status: SATISFIED
  solution: !Solution
    a:
    - o: 1
      s: null
***/

array [1..1] of record(var opt 1..2: s, var 1..1: o): a;
constraint absent(a[1].s);
