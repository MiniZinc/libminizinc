/***
!Test
extra_files:
- json_array2d_set.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  array2D: [!!set {}, !!set {}]
***/
array[1..2, 1..1] of set of int: array2D;