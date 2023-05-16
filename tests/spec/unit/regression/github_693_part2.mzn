/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x:
    - !!set {}
    - !!set {2}
    - !!set {1}
    - !Range 1..2
***/
include "all_different.mzn";

int: N = 4;
array [1..N] of var set of 1..N: x;

constraint all_different(x);
constraint array_union(x) = 1..(N div 2);
constraint forall(i in 1..N-1) ( x[i] < x[i+1] );
