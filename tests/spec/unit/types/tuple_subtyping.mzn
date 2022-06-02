/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    single: [1]
    double: [1.0, [1,2,3]]
    tupId: [1, 0.0]
    tupExpr: [-5.0, 0]
    arr1: [[5, 1.0], [1, -1.0]]
    arr2: [[1, 0], [1, 0]]
    arrId: [[1.0, 0.0], [1.0, 0.0]]
    arrExpr: [[1.0, 0.0], [2.0, 1.0], [3.0, 0.0]]
***/

tuple(int): single ::output = (true,);
tuple(float, array[int] of int): double ::output = (1, 1..3);

tuple(bool, bool): i1 = (true, false);
tuple(int, float): tupId ::output = i1;

function tuple(int, bool): tupFn() = (-5, false);
tuple(float, int): tupExpr ::output = tupFn();

array[1..2] of tuple(int, float): arr1 ::output = [(5, 1.0), (true, -1)];
array[1..2] of tuple(int, int): arr2 ::output = [(true, 0), (1, false)];

array[_] of tuple(float, float): arrId ::output = arr2;

function array[3..5] of tuple(int, bool): arrFn() = array1d(3..5, [
  (1, false),
  (2, true),
  (3, false),
]);
array[_] of tuple(float, float): arrExpr ::output = arrFn();