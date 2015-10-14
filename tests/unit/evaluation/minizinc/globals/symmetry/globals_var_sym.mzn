% RUNS ON mzn20_fd
% Test var_sym for var int.
include "all_different.mzn";
include "var_sym.mzn";

set of int: INDEX = 1..10;
array[INDEX] of var INDEX: x;

constraint all_different(x);
constraint var_sym([x[10], x[1], x[2], x[5], x[4],
                    x[3] , x[6], x[7], x[8], x[9]]);

solve satisfy;

output ["x = ", show(x)];
