% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

output [show(index_set_1of2([|_, _ | _, _|])), "\n"];
solve satisfy;
