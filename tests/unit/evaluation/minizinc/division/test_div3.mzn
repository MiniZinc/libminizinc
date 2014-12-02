% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
%% has solution i = 4 in relational and Kleene
%% no solution in strict 

array [1..3] of int: x = [1, 4, 9]; 

var 1..4: i; 

constraint i <= 3 -> x[i] > 10;

solve satisfy;

output ["i = ", show(i), ";\n"];
