/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !set {1}
***/

array[1..3] of set of int: C = [{1},{2},{3}];
array[int] of set of int: F(int: t) =
  if t = 3 then [] else [C[t]] endif;

any: x ::output = F(1);