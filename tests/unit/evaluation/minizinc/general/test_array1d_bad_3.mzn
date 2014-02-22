% RUNS ON mzn20_fd
array [1..3] of int: a :: is_output = array1d(1..3, [1, 2, 3, 4]);

solve satisfy;
