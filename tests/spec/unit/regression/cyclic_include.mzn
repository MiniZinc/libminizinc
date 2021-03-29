/***
!Test
solvers: [gecode]
expected: !Error
  regex: .*cyclic includes.*
***/

% Regression test for cyclic include detection

include "cyclic_include.mzn";
