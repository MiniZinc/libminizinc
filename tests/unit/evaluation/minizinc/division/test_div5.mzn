% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUNS ON mzn_cd_fd
%% has no solution in strict and Kleene or relational 

var 0..1: x; 

constraint 1 div x != 1; 

solve satisfy;

