/***
!Test
solvers: [gecode]
expected: !Result
  status: OPTIMAL_SOLUTION
  solution: !Solution
    idx: iE
    selected: {"a": 6, "b": {"c": 3, "d": 3}}
***/

include "strictly_increasing.mzn";

enum IDX = {iA, iB, iC, iD, iE};
array[IDX] of var record(2..6: a, record(1..10: c, 1..10: d): b): tups;
constraint strictly_increasing([tups[i].a | i in index_set(tups)]);
constraint forall(i in index_set(tups)) (tups[i].a = tups[i].b.c + tups[i].b.d);
constraint forall(i in index_set(tups)) (abs(tups[i].b.c - tups[i].b.d) <= 1);

var IDX: idx;
any: selected ::output = tups[idx];

solve maximize selected.a + selected.b.c + selected.b.d;