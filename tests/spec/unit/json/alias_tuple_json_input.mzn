/***
!Test
extra_files:
- alias_tuple_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  ps: [[1, "one"], [2, "two"]]
***/

type Pair = tuple(int, string);
array[int] of Pair: ps ::output;
