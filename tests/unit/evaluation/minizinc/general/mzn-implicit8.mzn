% RUNS ON minizinc_fd

array[int, int, int] of var 1..1: x
	= array3d(1..2, 1..2, 1..2, [_, _, _, _, _, _, _, _]);
array[int, int, int, int] of var 1..1: y
	= array4d(1..2, 1..2, 1..2, 1..2, 
          [_, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _]);
array[int, int, int, int, int] of var 1..1: z
	= array5d(1..2,1..2, 1..2, 1..2, 1..2, 
          [_, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
           _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _]);
solve satisfy;
output [
   "x = ", show(x), "\n",
   "y = ", show(y), "\n",
   "z = ", show(z)
];
