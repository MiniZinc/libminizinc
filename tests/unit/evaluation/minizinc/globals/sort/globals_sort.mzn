% RUNS ON mzn20_fd
include "sort.mzn";

%-----------------------------------------------------------------------------%
% sort
%-----------------------------------------------------------------------------%
array[1..7] of var -100..100: sort_avi;

constraint sort([3, 5, 1, 3, 5, 2, 8], sort_avi);

solve satisfy;

output ["sort_avi = array1d(1..7, ", show(sort_avi), ");\n"];
