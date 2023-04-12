/***
!Test
solvers: [gecode]
expected:
- !Result
  solution: !Solution
    x: null
    arr: [false]
- !Result
  solution: !Solution
    x: null
    arr: [true]
- !Result
  solution: !Solution
    x: 1
    arr: [false]
- !Result
  solution: !Solution
    x: 1
    arr: [true]
***/

var opt 1..1: x;
array[1..1] of var bool: arr;

constraint arr[1] -> true /\ arr[x] /\ exists(arr);
