/***
!Test
solvers: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    idx: iE
    selected: [5, 6]
***/

include "strictly_increasing.mzn";

enum IDX = {iA, iB, iC, iD, iE};
array[IDX] of var tuple(1..5, 1..10): tups;
constraint strictly_increasing([tups[i].1 | i in index_set(tups)]);
constraint forall(i in index_set(tups)) (tups[i].2 = tups[i].1 + 1);

var IDX: idx;
any: selected ::output = tups[idx];


solve maximize selected.1 + selected.2;