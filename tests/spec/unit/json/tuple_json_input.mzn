/***
!Test
extra_files:
- tuple_json_input.json
solvers: [gecode]
expected: !Result
  solution: !Solution
  singlet: [-3]
  duo: ["Albus", true]
  arr1d: [[0.2, true], [-23.1, false]]
  arr2d: [[[1, "one", true], [2, "two", true]], [[-1, "min one", false], [-2, "min two", false]]]
  empty_arr: []
***/


tuple(int): singlet ::output;
tuple(string, bool): duo ::output;
array[_] of tuple(float, bool): arr1d ::output;
array[_,_] of tuple(int, string, bool): arr2d ::output;
array[_] of tuple(bool, int, float, string): empty_arr :: output;