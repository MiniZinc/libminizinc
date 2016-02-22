% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

predicate foo(var int: x); 
var int: x;
var bool: b;
constraint b <-> foo(x);
solve satisfy;