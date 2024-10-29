/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    _output_item: "[1][1][1]\n"
***/

array[int, int] of int: foos = [| 1, 0 |];

output [
  show([ r[1] | t=foos, i in index_set_1of2(t), r=t[i, ..] ]), % error
  show([ r[1] | i in index_set_1of2(foos), t=foos, r=t[i, ..] ]), % error
  show([ r[1] | i in 1..1, t=foos, r=t[i, ..] ]), % error
  "\n",
];
