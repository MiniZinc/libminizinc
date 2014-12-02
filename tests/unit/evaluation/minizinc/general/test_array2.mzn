% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
%% has solution i = 4 in relational 
%% no solution in strict and Kleene 

array [1..3] of int: x = [1, 4, 9]; 

var 1..4: i; 

constraint x[i] < 11 -> i > 3;

solve satisfy;

output ["i = ", show(i), ";\n"];
