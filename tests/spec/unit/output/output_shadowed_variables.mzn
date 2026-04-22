/***
!Test
expected: !Result
  solution: !Solution
    i: i
    x: 1
    v: [1, 2, 3]
***/

array [int, int] of int: a = [|1, 2, 3 | 4, 5, 6 | 7, 8, 9|];
string: i :: output = "i";
var 1..3: x;
array [int] of var int: v :: output = [a[x, i] | i in 1..3];
solve minimize x;
