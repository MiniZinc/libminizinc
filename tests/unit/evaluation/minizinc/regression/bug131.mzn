% RUNS ON mzn_mer_fd
% RUNS ON mzn_mer_lazyfd

% Regression test for bug #131. 

var 1..3: x;
var 1..3: y;
var bool: b;

predicate foo(array[int] of var int: x);

predicate foo_reif(array[int] of var int: x, var bool: r) =
    r <-> forall(i,j in index_set(x) where i < j) ( x[i] != x[j] );

constraint foo([x,y]) <-> b;

solve satisfy;

output [
    "b = ", show(b), ";\n",
    "x = ", show(x), ";\n",
    "y = ", show(y), ";\n"
];
