/***
!Test
expected: !Result
  status: SATISFIED
  solution:  !Solution
    b: true
    x: 1
***/

% Regression test for a crash in the optimiser that didn't expect
% bool2int as a predicate

var bool: b;
var 0..1: x;
constraint bool2int(b,x);
constraint x = 1;
solve satisfy;
