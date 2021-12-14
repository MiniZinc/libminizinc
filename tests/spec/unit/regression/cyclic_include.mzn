/***
!Test
solvers: [gecode]
expected: !Error
  Type: CyclicIncludeError
***/

% Regression test for cyclic include detection

include "cyclic_include.mzn";
