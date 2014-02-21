% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

% Regression test: the frontend was not pushing the coercions into *both*
% arguments of the array1d cast.  Thiw was leading to mzn2fzn to failing
% because the types in the assignment did not match.

array[1..0] of var set of 1..10: x = array1d({}, []);
solve satisfy;
output [show(x)];
