% RUNS ON mzn-fzn_fd

array[int] of int: x = [1,2,3,4];

int: i = 2;
array[int] of int: xs = x[i..i+1];

output ["xs = ", show(xs), "\n"];
