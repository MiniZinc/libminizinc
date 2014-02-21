% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% has solution y = 1 in relational, Kleene and strict

var 0..2: y; 

constraint 2 div y + 2 div (y - 2) == 0 ; 

solve satisfy;

output ["y = ", show(y), ";\n"];
