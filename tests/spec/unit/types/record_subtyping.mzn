/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    single: {"a": 1}
    double: {"a": 1.0, "b": [1, 2, 3]}
    tupId: {"x": 1, "y": 0.0}
    tupExpr: {"a": -5.0, "b": 0}
    arr1: [{"a": 5, "b": 1.0}, {"a": 1, "b": -1.0}]
    arr2: [{"a": 0, "b": 1}, {"a": 0, "b": 1}]
    arrId: [{"a": 0.0, "b": 1.0}, {"a": 0.0, "b": 1.0}]
    arrExpr: [{"a": 1.0, "b": 0.0}, {"a": 2.0, "b": 1.0}, {"a": 3.0, "b": 0.0}]
***/

record(int: a): single ::output = (a: true,);
record(float: a, array[int] of int: b): double ::output = (a: 1, b: 1..3);

record(bool: x, bool: y): i1 = (x: true, y: false);
record(int: x, float: y): tupId ::output = i1;

function record(int: a, bool: b): tupFn() = (a: -5, b: false);
record(int: b, float: a): tupExpr ::output = tupFn();

array[1..2] of record(int: a, float: b): arr1 ::output = [(a: 5, b: 1.0), (a: true, b: -1)];
array[1..2] of record(int: a, int: b): arr2 ::output = [(b: true, a: 0), (b: 1, a: false)];

array[_] of record(float: a, float: b): arrId ::output = arr2;

function array[3..5] of record(int: a, bool: b): arrFn() = array1d(3..5, [
  (a: 1, b: false),
  (a: 2, b: true),
  (a: 3, b: false),
]);
array[_] of record(float: b, float: a): arrExpr ::output = arrFn();