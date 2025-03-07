/***
!Test
solvers: [gecode]
expected: !Result
  - !Solution
    arr: [2, 1]
***/
int: n = 2;
int: m = 4;
array[1..n] of var 1..m: soln;

constraint forall(i in 1..n-1)(soln[i] > soln[i+1]);
var int: total = sum(i in 1..n)(soln[i]);

solve
 :: warm_start(soln[1..n], [2,1])
  minimize total;
