% RUNS ON minizinc_fd

constraint let {
        array[int] of int: x = [1, 2, 3, 4]
    } in (
	length(x) = 4
    );

solve satisfy;

output ["Ok"];
