% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

string: s = join(", ", ["a", "b", "c", "d"]);
solve satisfy;
output [s];
