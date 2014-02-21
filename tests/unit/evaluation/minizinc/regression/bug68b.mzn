% RUNS ON mzn_mer_fd

% Regression test for the second problem in bug 68.
% The type checker was assigning the type var bottom to the elements of
% array1d(1..2, [_, _]) below and that was causing mzn2fzn to abort.

int: N = 2;
array[1..N] of var 1 .. N: b;
constraint forall([ b[i] = array1d(1..2, [_, _])[i] | i in index_set(b) ]);
solve satisfy;
output ["b = ", show(b), "\n"];
