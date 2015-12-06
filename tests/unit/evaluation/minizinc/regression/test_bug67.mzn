% RUNS ON mzn20_fd

predicate foo(var int: x); 
var int: x;
var bool: b;
constraint b <-> foo(x);
solve satisfy;