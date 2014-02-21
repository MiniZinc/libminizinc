% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

var -10..10: x = length([_, _, _]);
solve satisfy;
output([show(x), "\n"]);
