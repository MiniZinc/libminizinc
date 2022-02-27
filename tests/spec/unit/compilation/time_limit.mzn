/***
--- !Test
type: compile
solvers: [gecode]
options:
  --time-limit: 1
expected: !Error
  message: 'time limit reached'
***/

% In mzn2fzn mode, timeouts throw an error (so that the timeout can be detected)

int: n = 10000;
array [1..n] of var 1..n: x;
constraint forall (i, j in 1..n where i != j) (x[i] != x[j]);
