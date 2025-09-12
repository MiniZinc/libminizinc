/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

list of tuple(
  int, array[1..1] of opt 1..
): A = [
  (2, [1]),
  (1, [<>]),
];

list of tuple(
  int, array[1..1] of 0.. % <- Opt not allowed by type
): B = A ++ [
  (1, [0])
];

output join("\n", [
  "A:\(A)",
  "B:\(B)",
]);