/***
!Test
extra_files:
- alias_chained_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  ps: [[1, "one"], [2, "two"]]
***/

type Pair = tuple(int, string);
type Pairs = array[int] of Pair;
Pairs: ps ::output;
