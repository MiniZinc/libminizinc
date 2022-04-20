/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    single: [1]
    double: [false, -1]
    triple: [1, false, 200.0]
    square: [[1, "one"], [-2, "two"]]
***/

tuple(int): single ::output = (1,);
tuple(bool, int): double ::output = (false, -1);
tuple(int, bool, float): triple ::output = (1, false, 2.0e2,);

array[1..2] of tuple(int, string): square ::output = [(1, "one"), (-2, "two")];