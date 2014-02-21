% RUNS ON minizinc_fd

array[1..2, int] of var 1..1: x = [|_, _|_, _|];
solve satisfy;
output [show(x)];
