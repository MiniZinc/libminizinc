/***
!Test
solvers: [gecode]
expected:
- !Result
  status: UNSATISFIABLE
***/

% A fixed-false assumption makes the model trivially unsatisfiable.
include "experimental/assume.mzn";
var bool: p;
constraint assume([p, 2 > 3]);
solve satisfy;
