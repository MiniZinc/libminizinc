% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

var set of 1..1: x;
constraint 1 in x;

var bool: b;
constraint b = exists([true | s in x]);

solve satisfy;