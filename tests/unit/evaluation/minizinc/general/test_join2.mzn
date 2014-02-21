% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

string: s = join(", ", [show(10), show(5.3), "A String", "foo" ++ "bar"]);
solve satisfy;
output [s];
