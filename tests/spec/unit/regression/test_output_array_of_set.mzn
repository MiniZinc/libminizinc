/***
!Test
expected: !Result
  status: SATISFIED
***/

% This used to cause the error: 'MiniZinc: internal error: invalid set literal type'

array[1..1] of var set of 1..1: sets;
output [show(sets[i]) | i in 1..1];