% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
var 1..10: x;
constraint not( let { var 1..10: y = x + 1 } in x = y );
solve satisfy;
output ["x = ", show(x), ";\n"];
