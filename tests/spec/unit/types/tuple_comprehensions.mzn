/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    simple: [[2, true], [1, false], [0, true], [1, false], [2, true]]
    indexed: [[4, true], [6, true], [8, true], [9, false], [11, false]]
***/

array[_] of tuple(int, bool): simple ::output = [
  (abs(i), abs(i) mod 2 = 0) | i in -2..2
];

enum Some = {iA, iB, iC, iD, iE};
array[_] of int: some_map = [iA: 4, iB: 6, iC: 8, iD: 9, iE: 11];

array[_] of tuple(int, bool): indexed ::output = [
  i: (some_map[i], some_map[i] mod 2 = 0) | i in Some
]