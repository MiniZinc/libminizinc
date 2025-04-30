/***
!Test
solvers: [gecode]
expected: !Result
- !Solution
  i: [0,0]
  j: [[0,0]]
***/

array[1..2] of var 0..0: i;
array[_] of var tuple(int, int): j ::output = [k | k in [if sum(i) > 1 then (0, 0) else (0, 0) endif]];
