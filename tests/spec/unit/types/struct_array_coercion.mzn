/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    mzn_enum_X: [["A", []], ["B", []], ["G", [[0, !Range 1..3]]]]
***/

array [int] of tuple(string, array [int] of tuple(int, set of int)): mzn_enum_X :: output;
mzn_enum_X = [("A", []),("B", []),("G", [(0, 1..3)])];
