% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd

% Check that the MiniZinc round builtin operation.
% (mzn2fzn 1.1.3 didn't implement it.)
%
par float: x = 3.6;
var int: y = round(x);
solve satisfy;
output ["y = ", show(y), ";\n"];
