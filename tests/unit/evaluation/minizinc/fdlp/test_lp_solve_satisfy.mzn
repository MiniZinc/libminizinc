% RUNS ON mzn20_fd
var 0..9: X;
constraint X > 5;
solve satisfy;
output ["X = ", show(X), ";\n"];
