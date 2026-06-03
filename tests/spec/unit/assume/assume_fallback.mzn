/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    p: true
    q: true
***/

% On a solver without native assumption support, the assumptions are posted as
% hard constraints (with a warning), so the assumed Booleans are forced to true.
include "experimental/assume.mzn";
var bool: p;
var bool: q;
constraint assume([p, q]);
solve satisfy;
