% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
var 0..9: X;
constraint X > 5;
solve satisfy;
output ["X = ", show(X), ";\n"];
