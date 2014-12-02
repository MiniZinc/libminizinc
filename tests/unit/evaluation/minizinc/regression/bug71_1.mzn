% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

var int: x;

constraint x = min (i in 3..2) ([1, 2, 3][i]);

solve satisfy;
