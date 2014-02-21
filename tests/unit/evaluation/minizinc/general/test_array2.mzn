% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS not ON mzn_cd_fd
% RUNS ON mzn_mer_lp
%% has solution i = 4 in relational 
%% no solution in strict and Kleene 

array [1..3] of int: x = [1, 4, 9]; 

var 1..4: i; 

constraint x[i] < 11 -> i > 3;

solve satisfy;

output ["i = ", show(i), ";\n"];
