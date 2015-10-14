% RUNS ON mzn20_fd
% Test var_perm_sym for var int.
% todo: make better test
include "var_perm_sym.mzn";
include "all_different.mzn";

set of int: INDEX = 1..10;
array[INDEX] of var INDEX: x;
array[1..2,INDEX] of int:
  ps = [|  6,  7,  8,  2,  3,  1, 10,  4,  5,  9
        | 10,  2,  3,  4,  5,  6,  7,  8,  9,  6 |];

constraint var_perm_sym(x, ps);
constraint all_different(x);

solve satisfy;

output ["x = ", show(x)];
