/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    simple: [{"x": 2, "y": true}, {"x": 1, "y": false}, {"x": 0, "y": true}, {"x": 1, "y": false}, {"x": 2, "y": true}]
    indexed: [{"a": 4, "b": true}, {"a": 6, "b": true}, {"a": 8, "b": true}, {"a": 9, "b": false}, {"a": 11, "b": false}]
***/

array[_] of record(int: x, bool: y): simple ::output = [
  (x: abs(i), y: abs(i) mod 2 = 0) | i in -2..2
];

enum Some = {iA, iB, iC, iD, iE};
array[_] of int: some_map = [iA: 4, iB: 6, iC: 8, iD: 9, iE: 11];

array[_] of record(int: a, bool: b): indexed ::output = [
  i: (a: some_map[i], b: some_map[i] mod 2 = 0) | i in Some
]