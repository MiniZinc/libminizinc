% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear

% Regression test for github bug #8

var 1..2: x;
var int : nx = -x;
solve minimize nx;
