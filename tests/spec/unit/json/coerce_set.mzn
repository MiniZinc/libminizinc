/***
!Test
extra_files:
- coerce_set.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  s: !!set {1, 2, 3}
***/

set of int: s ::add_to_output;
