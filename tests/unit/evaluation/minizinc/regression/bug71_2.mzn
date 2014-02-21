var int: x :: is_output;

constraint (x = min (i in 3..2) ([1, 2, 3][i]) \/ x = 1);

solve satisfy;
