/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

list of tuple(
  int,
  array[1..1] of opt 1..
): A = [
  (1, [ <> ]),
];

list of tuple(
  int,
  array[1..1] of 0..
): B = A ++ [
  (1, [ 0 ]),
];

array[int, int] of int: T =
array2d(1..2, 1..2,
[ ([ tp.1, ] ++ tp.2)[j]
| tp in B, j in 1..2 ]);

output join("\n", [
  "A:\(A)",
  "B:\(B)",
  "T:\(T)",
]);
