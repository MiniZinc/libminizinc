% RUNS ON mzn_mer_fd
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

var 1..3: a;

predicate test_pred(1.0..10.0: x, var int: y) = y = 3;

constraint test_pred(11.0, a) == false;

solve satisfy;

output ["a = ", show(a), ";\n"];
