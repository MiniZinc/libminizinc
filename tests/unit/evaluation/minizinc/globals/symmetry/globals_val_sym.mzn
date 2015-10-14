% RUNS ON mzn20_fd
% Test val_sym for var int.
include "val_sym.mzn";
include "count.mzn";

set of int: INDEX = 1..10;
array[INDEX] of var INDEX: x;

constraint val_sym([x[10], x[8], x[7], x[1], x[2],
                    x[4],  x[3], x[6], x[5], x[4]], INDEX);
constraint count(x, 1, 2);
constraint count(x, 2, 2);
constraint count(x, 3, 2);
constraint count(x, 4, 2);
constraint count(x, 5, 2);
constraint x[1] = x[4];
constraint x[5] = x[3];

solve satisfy;

output ["x = ", show(x)];
