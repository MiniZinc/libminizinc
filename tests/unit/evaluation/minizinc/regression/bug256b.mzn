% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

% A regression test for bug #256.

array[1..3] of var set of 1..3: x;
x = array1d(1..3, [{}, {}, {}]);
solve satisfy;
output ["x = ", show(x), "\n"];
