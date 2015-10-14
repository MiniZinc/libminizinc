% RUNS ON mzn20_fd
% Test var_perm_sym_ord for var int.
include "var_perm_sym.mzn";
include "all_different.mzn";

set of int: INDEX = 1..4;
array[INDEX] of var INDEX: x;
array[1..2,INDEX] of int:
  p1 = [| 1, 2, 3, 4
        | 2, 3, 4, 1 |];
array[INDEX] of var INDEX: y = [x[2], x[1], x[3], x[4]];

constraint all_different(x);

% enforces lex_lesseq(x, [x[2], x[3], x[4], x[1]])
constraint var_perm_sym(x, p1);

% without ordering this enforces
% lex_lesseq([x[2], x[1], x[3], x[4]], [x[1], x[3], x[4], x[2]])
% which is unsatisfiable with the above constraints
%constraint var_perm_sym(y, p1);

solve satisfy;
output [];
