/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x:
    - - !!set {1, 2}
      - !!set {1, 2}
    - - !!set {1, 2}
      - !!set {1, 2}
***/

% set cardinality applied through an array-of-array of var sets
array[1..2] of array[1..2] of var set(2..2) of 1..5: x ::output;
constraint forall (i,j in 1..2) (1 in x[i][j] /\ 2 in x[i][j]);

solve satisfy;
