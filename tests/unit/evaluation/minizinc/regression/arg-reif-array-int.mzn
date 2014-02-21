% RUNS ON mzn_mer_fd
% RUNS ON minizinc_fd
% RUNS ON minizinc_cpx

var 1..3: a;

predicate test_pred(array[int] of 1..10: x, var int: y) = y = 3;

constraint test_pred([1, 2, 4, 11], a) == false;

solve satisfy;

output ["a = ", show(a), ";\n"];
