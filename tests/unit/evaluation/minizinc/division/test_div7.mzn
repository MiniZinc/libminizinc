% RUNS ON mzn20_fd
%% has solution y = 1 in relational and Kleene and strict 

var 0..1: y; 

constraint not(y < 1 \/ 1 div y != 1); 

solve satisfy;

output ["y = ", show(y), ";\n"];
