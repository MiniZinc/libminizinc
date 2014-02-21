% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS ON mzn_cd_fd
array [1..2] of var set of 0..9: x = [{0}, _];
solve satisfy;
output ["x = ", show(x), ";\n"];
