% RUNS ON minizinc_fd

array[int] of int: x = [];
solve satisfy;
output [show(x)];
