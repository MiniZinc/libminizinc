% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS not ON mzn_cd_fd
%% has no solution in strict and Kleene 
%% solution x = 0 in relational 

var 0..1: x; 

constraint not (1 div x = 1); 

solve satisfy;

output ["x = ", show(x), ";\n"];
