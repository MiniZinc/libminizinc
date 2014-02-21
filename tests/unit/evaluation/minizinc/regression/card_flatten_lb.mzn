% RUNS ON mzn_fd
% RUNS ON mzn_mer_fd

% Prior to r9313, mzn2fzn would "improve" the bounds on y incorrectly.
% In particular the lower bound would be set to 1 rather than 0.
%
var set of 1..15: x;
var -100..100: y;
constraint y = card(x);
solve minimize y;
output ["y = ", show(y), ";\n"];
