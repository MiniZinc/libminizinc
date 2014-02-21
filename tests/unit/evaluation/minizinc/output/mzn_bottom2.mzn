% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

output [show(index_set([_, _, _])), "\n"];
solve satisfy;
