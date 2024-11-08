/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    y: [[1, 2, 3]]
***/

any: x = [
    [1, 2, 3],
    [4, 5, 6]
];

any: y :: output = x[1..1];
