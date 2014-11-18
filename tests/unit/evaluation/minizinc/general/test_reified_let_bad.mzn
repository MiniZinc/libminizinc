% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip
var 1..10: x;
var bool: b;
constraint b <-> ( let { var 1..10: y } in x = y );
solve satisfy;
