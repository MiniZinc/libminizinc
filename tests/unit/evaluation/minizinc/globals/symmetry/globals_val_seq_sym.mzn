% RUNS ON mzn20_fd
% Test val_sym for var int.
% TODO: make better test
include "val_seq_sym.mzn";
include "count.mzn";

set of int: INDEX = 1..10;
array[INDEX] of var INDEX: x;
array[1..3,1..2] of int:
  s = [|  1,  4
       |  2,  5
       | 10,  9 |];

constraint val_seq_sym(x, s);
constraint forall(i in 1..5)(count(x, i, 2));

solve satisfy;

output ["x = ", show(x)];
