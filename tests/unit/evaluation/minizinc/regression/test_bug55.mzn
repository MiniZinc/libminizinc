% RUNS ON mzn20_fd
% RUNS ON mzn-fzn_fd

set of int: X = -1..1;
set of int: Y = 0..3;

var Y: y;

function var X: f( var Y: y ) = y+1;

var int: x = f(y);

solve satisfy;

output [ show(x) ];