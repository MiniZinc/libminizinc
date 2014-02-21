% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

string: s = concat([show(10), show(5.3), show({1, 2, 3}), "A String"]);
solve satisfy;
output [s];
