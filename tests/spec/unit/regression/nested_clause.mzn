/***
!Test
expected:
- !Result
  solution: !Solution
    x: 2
    y: [false, false]
***/

var 2..2: x;
array[1..2] of var bool: y;

constraint (1 <= x) -> (not y[1] \/ not y[2]);

solve satisfy;
