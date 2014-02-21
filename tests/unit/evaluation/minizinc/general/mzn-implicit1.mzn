% RUNS ON mzn_mer_fd
% RUNS ON minizinc_fd

array[int] of var 1..1: x = [_, _, _];
solve satisfy;
output ["x = array1d(1..3, ", show(x), ");"];
