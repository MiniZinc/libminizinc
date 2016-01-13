% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
%% has no solution in strict and Kleene or relational 

var 0..1: x; 

constraint 1 div x != 1; 

solve satisfy;

