% RUNS ON minizinc_fd

array[int, int] of var 1..1: x = array2d(1..2, 3..4, [_, _, _, _]);
set of int: ix = index_set_2of2(x);
solve satisfy;
output [
   "x = ", show(x), "\n",
   "ix = ", show(ix)
];
