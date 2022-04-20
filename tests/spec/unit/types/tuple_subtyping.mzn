/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    single: [1]
    double: [1.0, [1,2,3]]
    arr1: [[5, 1.0], [1, -1.0]]
    arr2: [[1, 0], [1, 0]]
    arr3: [[2,2], [3,3,3]]
***/

tuple(int): single ::output = (true,);
tuple(float, array[int] of int): double ::output = (1, 1..3);

constraint double.2[1] == 1;

array[1..2] of tuple(int, float): arr1 ::output = [(5, 1.0), (true, -1)];
array[1..2] of tuple(int, int): arr2 ::output = [(true, 0), (1, false)];
array[1..2] of tuple(int, int): arr3 ::output = [(2, 2), (3, 3, 3)];