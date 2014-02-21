% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

output [show(index_set([])), "\n"];
solve satisfy;
