% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

% Regression test for a crash in the optimiser that didn't expect
% bool2int as a predicate

var bool: b;
var 0..1: x;
constraint bool2int(b,x);
constraint x = 1;
solve satisfy;
