/***
!Test
expected:
- !Result
  solution: !Solution
    sum_s: 13
***/

include "sum_pred.mzn";

%-----------------------------------------------------------------------------%
% sum_pred
%-----------------------------------------------------------------------------%

% The expected answer for 'sum_s' is 13.
var int: sum_s ::add_to_output; 

constraint
    sum_pred(3, [{}, {2,3}, {1,3,4}, {4,6}], [1, 3, 5, 7, 9, 11], sum_s);

solve satisfy;

output ["sum_s = ", show(sum_s), ";\n"];
