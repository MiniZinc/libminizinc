% RUNS ON mzn20_fd

set of int: smallset = 0..10;
array[1..2, 1..4] of var smallset: X;
constraint forall( i in 1..2 ) (
let {
  array[int] of var smallset: Y = row(X, i);
} in
  Y[1] == 3 % Any use of Y triggers the issue.
);
solve satisfy;