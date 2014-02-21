% RUNS ON minizinc_fd

array[int, int] of int: a = [|1, 2|3, 4|];
set of int: ax = index_set_1of2(a);
set of int: bx = index_set_2of2(a);
solve satisfy;
output [
    "ax = ", show(ax), "\n",
    "bx = ", show(bx)
];
