% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lp
% RUNS not ON mzn_cd_fd
%% has solution y = 1 in relational, Kleene 
%% no solutions in strict

var 0..1: y; 

constraint y == 1 \/ 2 div y + 2 div (y - 1) == 0 ; 

solve satisfy;

output ["y = ", show(y), ";\n"];
