/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*cyclic include.*
***/

% Regression test for cyclic include detection

include "cyclic_include.mzn";
