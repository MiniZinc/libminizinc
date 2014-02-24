% RUNS ON mzn20_fd

% A regression test for bug #256.

array[1..3] of var set of 1..3: x;
x = array1d(1..3, [{}, {}, {}]);
solve satisfy;
output ["x = ", show(x), "\n"];
