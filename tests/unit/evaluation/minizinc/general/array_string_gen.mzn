% RUNS ON minizinc_fd

% Test using arrays of strings as generators.

array[1..3] of string: something = ["foo", "bar", "baz"];
array[1..3] of string: x = [s ++ "s" | s in something];
solve satisfy;
output x;
