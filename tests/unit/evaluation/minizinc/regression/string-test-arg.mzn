% RUNS ON mzn20_fd
% RUNS ON mzn20_fd_linear
% RUNS ON mzn20_mip

% mzn2fzn r14673 and before aborted on this model due to the test predicate
% having a string argument.

bool: b = true;

string: s = "bar";

test foo(string: i) = i = "foo" /\ b;

var int: x = if foo(s) then 3 else 4 endif;

solve satisfy;

output ["x = ", show(x), ";\n"];
