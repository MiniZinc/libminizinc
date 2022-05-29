/***
!Test
solvers: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    idx: iE
    selected: [6, [3, 3]]
***/

include "strictly_increasing.mzn";

enum IDX = {iA, iB, iC, iD, iE};
array[IDX] of var tuple(2..6, tuple(1..10, 1..10)): tups;
constraint strictly_increasing([tups[i].1 | i in index_set(tups)]);
constraint forall(i in index_set(tups)) (tups[i].1 = tups[i].2.1 + tups[i].2.2);
constraint forall(i in index_set(tups)) (abs(tups[i].2.1 - tups[i].2.2) <= 1);

var IDX: idx;
any: selected ::output = tups[idx];

solve maximize selected.1 + selected.2.1 + selected.2.2;