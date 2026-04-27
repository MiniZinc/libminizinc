/***
!Test
solvers: [gecode]
type: solve
expected: !Result
  solution: !Solution
    v: A
***/

enum E = { A, B };

array[1..2] of record(E: x, E: y): tab = [
  (x: A, y: A),
  (x: B, y: B),
];

var E: v;

constraint (x: v, y: A) in tab;

solve satisfy;
