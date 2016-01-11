% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
%% has solution y = 0 in relational and Kleene 
%% no solution in strict 

var 0..1: y; 

constraint y < 1 \/ 1 div y != 1; 

solve satisfy;

output ["y = ", show(y), ";\n"];
