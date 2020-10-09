/***
!Test
expected:
- !Result
  solution: !Solution
    sort_avi: [1, 2, 3, 3, 5, 5, 8]
***/

include "sort.mzn";

%-----------------------------------------------------------------------------%
% sort
%-----------------------------------------------------------------------------%
array[1..7] of var -100..100: sort_avi ::add_to_output;

constraint sort([3, 5, 1, 3, 5, 2, 8], sort_avi);

solve satisfy;

output ["sort_avi = array1d(1..7, ", show(sort_avi), ");\n"];
