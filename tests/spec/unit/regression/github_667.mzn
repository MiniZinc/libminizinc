/***
!Test
solvers: [gecode, chuffed, highs]
expected: !Result
  solution: !Solution
    arr: [false]
***/

array[1..1] of var bool: arr;
constraint not exists(arr[1..1]);