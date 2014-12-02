% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
var 0..9: X;
constraint X > 5;
solve satisfy;
output ["X = ", show(X), ";\n"];
