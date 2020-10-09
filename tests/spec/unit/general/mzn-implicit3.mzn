/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    ax: !Range 1..2
    bx: !Range 1..2
***/

array[int, int] of int: a = [|1, 2|3, 4|];
set of int: ax :: add_to_output = index_set_1of2(a);
set of int: bx :: add_to_output = index_set_2of2(a);
solve satisfy;
output [
    "ax = ", show(ax), "\n",
    "bx = ", show(bx)
];
