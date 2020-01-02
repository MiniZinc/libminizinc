/***
!Test
expected:
- !Result
  solution: !Solution
    x: []
***/

% mzn2fzn and soln2out aborted when evaluating this model in
% r14647 and before.

array[1..0] of var bool: x;
x = array1d(1..0, []);
solve satisfy;
output ["x = ", show(x), "\n"];
