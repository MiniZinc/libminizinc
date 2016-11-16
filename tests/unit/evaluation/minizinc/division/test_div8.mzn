% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
%% has solution y = 0, y = 1 in relational
%% no solution in Kleene and strict 

var 0..1: y; 

constraint not ( 2 div y + 2 div (y - 1) == 0 ); 

solve satisfy;

output ["y = ", show(y), ";\n"];

