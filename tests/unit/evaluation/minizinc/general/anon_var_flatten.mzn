% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
array [1..2] of var set of 0..9: x = [{0}, _];
solve satisfy;
output ["x = ", show(x), ";\n"];
