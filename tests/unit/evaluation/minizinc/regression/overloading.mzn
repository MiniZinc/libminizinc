% RUNS ON mzn20_fd
% RUSN ON mzn_mer_lp
% bug #1:  the overloading was being handled wrongly, giving the wrong output
% "b = true".
predicate foo(set of int: x) = true;
predicate foo(int: x) = false;
var bool: b1 = foo(3);
var bool: b2 = foo({42});
solve satisfy;
output ["b1 = ", show(b1), "; b2 = ", show(b2), "\n"];
