% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

% Regression test for github bug #8

var 1..2: x;
var int : nx = -x;
solve minimize nx;
