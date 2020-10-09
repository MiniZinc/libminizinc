/***
!Test
expected:
- !Result
  solution: !Solution
    _output_item: 'Ok'
***/

% Regression test for bug #288: in the absence of a final newline in the
% output item, solns2out was not outputting the soluion separators on
% separate line.

var 1..10: x;
solve satisfy;
output ["Ok"];
