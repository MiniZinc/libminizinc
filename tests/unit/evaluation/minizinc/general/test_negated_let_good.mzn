% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
% RUNS ON mzn20_mip
var 1..10: x;
constraint not( let { var 1..10: y = x + 1 } in x = y );
solve satisfy;
output ["x = ", show(x), ";\n"];
