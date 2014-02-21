% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS not ON mzn_cd_fd
%% has solution y = 0 in relational and Kleene 
%% no solution in strict 

var 0..1: y; 

constraint y < 1 \/ 1 div y != 1; 

solve satisfy;

output ["y = ", show(y), ";\n"];
