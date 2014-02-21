% RUNS ON minizinc
% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd
% RUSN ON mzn_mer_lp
% RUNS ON mzn_cd_fd
% bug #1:  the overloading was being handled wrongly, giving the wrong output
% "b = true".
predicate foo(set of int: x) = true;
predicate foo(int: x) = false;
var bool: b1 = foo(3);
var bool: b2 = foo({42});
solve satisfy;
output ["b1 = ", show(b1), "; b2 = ", show(b2), "\n"];
