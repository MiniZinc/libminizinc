/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: 'Ok'
***/

% Regression test for bug #288: (see bug288a.mzn for details).  Check
% that we do not get double newlines printed when the output expression
% _is_ terminated by a newline.

var 1..10: x;
solve satisfy;
output ["Ok"];
