% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

int: x = length([]) + length([]);
solve satisfy;
output([show(x), "\n"]);
