/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: |
      Ok
      
***/

% Regression test for bug #288: (see bug288a.mzn for details).  Check
% that empty strings at the end of an output item do not affect the
% detection of whether or not there is a terminating newline.

var 1..10: x;
solve satisfy;
output ["Ok", "\n", "" ++ ""];
