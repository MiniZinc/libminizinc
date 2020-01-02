/***
!Test
expected:
- !Result
  solution: !Solution
    cum_bound: 6
***/

include "cumulative.mzn";

%-----------------------------------------------------------------------------%
% cumulative
%-----------------------------------------------------------------------------%
var 1..6: cum_bound;

constraint cumulative([0, 3, 4, 6, 8, 8],  % start
                      [3, 2, 5, 2, 1, 4],  % duration
                      [2, 3, 1, 4, 3, 2],  % resource usage
                      cum_bound);

solve satisfy;

output ["cum_bound = ", show(cum_bound), ";\n"];
