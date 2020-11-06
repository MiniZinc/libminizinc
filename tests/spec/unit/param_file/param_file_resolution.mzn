/***
---
!Test
solvers: [gecode]
extra_files:
- param_file_resolution.mpc
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: 1
    y: 2
***/

include "included.mzn";
