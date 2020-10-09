/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    b: [true, false, false]
***/

array [1..3] of var bool: b;
constraint b[1] = true;
solve satisfy;
