/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution {}
  status: SATISFIED
***/

% `++` binds tighter than `+`, `*`, `^` and `not` (parser.yxx puts MZN_PLUSPLUS
% above all of them). Overloading `+` on arrays makes the grouping observable:
% `p + (q ++ r)` applies the overload elementwise and yields two elements, while
% `(p + q) ++ r` would yield four and make this unsatisfiable.
function array[int] of int: '+'(array[int] of int: a, array[int] of int: b) =
  [ a[i] + b[i] | i in index_set(a) ];

array[int] of int: p = [1, 2];
array[int] of int: q = [10, 20];
array[int] of int: r = [100, 200];
array[int] of int: res = p + q ++ r;

constraint res = [11, 22];

solve satisfy;
