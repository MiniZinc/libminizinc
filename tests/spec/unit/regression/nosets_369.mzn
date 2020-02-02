/***
!Test
expected: !Result
  status: SATISFIED
***/

% Regression test for wrong index set in nosets.mzn (part of GitHub issue #369)

include "nosets.mzn";

constraint set_lt({}, {0});