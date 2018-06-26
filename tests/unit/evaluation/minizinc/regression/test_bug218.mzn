% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

include "test_bug218_inc.mzn";
var 1..ub(sum(x)): a;
array[1..3] of var 1..3: y;
solve satisfy;
