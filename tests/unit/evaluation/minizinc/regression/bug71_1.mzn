% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

var int: x;

constraint x = min (i in 3..2) ([1, 2, 3][i]);

solve satisfy;
