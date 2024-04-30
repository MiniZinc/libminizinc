/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    bar: [1]
***/

% Previously the RHS of assignment generators were not coerced, so the slicing failed

array[int, int] of int: foos = [| 1, 0 |];

array[int] of int: bar :: output = [
  r[1] | i in index_set_1of2(foos), r = foos[i, ..]
];
