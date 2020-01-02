/***
!Test
expected: !Result
  solution: !Solution
    x: [[1, 1], [1, 1]]
***/

array[1..2, int] of var 1..1: x :: add_to_output = [|1, _|_, _|];
solve satisfy;