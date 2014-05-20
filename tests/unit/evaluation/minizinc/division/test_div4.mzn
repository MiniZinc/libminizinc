% RUNS ON mzn20_fd
%% has solution x = 1 in relational, strict and Kleene 

var 0..1: x; 

constraint not (not (1 div x = 1)); 

solve satisfy;

output ["x = ", show(x), ";\n"];

