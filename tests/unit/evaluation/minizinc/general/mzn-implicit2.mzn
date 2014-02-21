% RUNS ON minizinc_fd

array[int] of int: a = [1, 2, 3];
set of int: ia = index_set(a);

array[int] of int: b = array1d(2..5, [1, 2, 3, 4]);
set of int: ib = index_set(b);

solve satisfy;

output [
   "ia = ", show(ia), "\n",
   "ib = ", show(ib),
];
