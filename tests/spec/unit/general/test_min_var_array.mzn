/***
!Test
expected:
- !Result
  solution: !Solution
    a: [1, 1, 1]
    x: 1
- !Result
  solution: !Solution
    a: [2, 2, 2]
    x: 2
- !Result
  solution: !Solution
    a: [3, 3, 3]
    x: 3
***/

array [1..3] of var 1..3: a :: add_to_output;
var int: x :: add_to_output = min(a);

solve maximize x;
