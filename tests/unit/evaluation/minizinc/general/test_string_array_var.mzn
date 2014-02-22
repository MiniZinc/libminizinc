% RUNS ON mzn20_fd
array [1..2] of string: s = ["All", " is"];
solve satisfy;
output s ++ [" well\n"];
