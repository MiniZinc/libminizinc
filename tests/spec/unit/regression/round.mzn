/***
!Test
expected:
- !Result
  solution: !Solution
    y: 4
***/

% Check that the MiniZinc round builtin operation.
% (mzn2fzn 1.1.3 didn't implement it.)
%
par float: x = 3.6;
var int: y :: add_to_output = round(x);
solve satisfy;
output ["y = ", show(y), ";\n"];
