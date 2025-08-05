/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

array[1..0] of var int: aaa;

solve :: seq_search([] ++ [ bool_search([], input_order, indomain_min) ]) satisfy;
