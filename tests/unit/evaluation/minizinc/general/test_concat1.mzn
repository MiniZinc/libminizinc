% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

string: s = concat(["abc", "def", "ghi"]);
solve satisfy;
output [show(s)];
