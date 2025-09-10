/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

include "ide/vis.mzn";

array[1..3] of var int: x;
any: y = reverse(x);
array[int] of int: s = reverse([1,2,3]);
any: y2 = reverse(x);

array[1..3] of var 1..3: z;

output vis_geost_2d(
    [1,2,3], [1,2,3],
    s, [1,2,3],
    [{1},{2},{3}],
    [1,2,3], z,
    [1,2,3]
);
