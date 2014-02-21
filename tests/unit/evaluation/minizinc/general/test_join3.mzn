% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

array[1..4] of string: v;
string: s = join(", ", v);
v = [show(10), show(5.3), "A String", "foo" ++ "bar"];
solve satisfy;
output [s];
