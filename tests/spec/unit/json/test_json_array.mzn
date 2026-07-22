/***
!Test
extra_files:
- test_json_array.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  x: [1, 2, 3, 4]
  y: [[1, 2], [3, 4], [5, 6]]
  z: [[[1],[2]]]
***/

list of int: x ::output;
array[1..3,1..] of int: y ::output;
array[1..1,int,1..1] of int: z ::output;
