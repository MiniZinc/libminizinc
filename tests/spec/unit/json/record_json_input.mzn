/***
!Test
extra_files:
- record_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  singlet: [-3]
  duo: ["Albus", true]
  arr1d: [[0.2, true], [-23.1, false]]
  arr2d: [[[1, "one", true], [2, "two", true]], [[-1, "min one", false], [-2, "min two", false]]]
  empty_arr: []
  mixed_arr: [[1, 2], [3, null], [5, null]]
***/


record(int: x): singlet ::output;
record(string: x, bool: y): duo ::output;
array[_] of record(float: e, bool: b): arr1d ::output;
array[_,_] of record(int: x1, string: x2, bool: x3): arr2d ::output;
array[_] of record(bool: a, int: b, float: c, string: d): empty_arr :: output;
array[_] of record(int: x, opt int: y): mixed_arr ::output;
