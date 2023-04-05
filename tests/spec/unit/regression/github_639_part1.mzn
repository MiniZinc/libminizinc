/***
!Test
options:
  all_solutions: true
solvers: [gecode]
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    arr: [null, null]
  - !Solution
    x: 1
    arr: [1, null]
  - !Solution
    x: 2
    arr: [null, null]
  - !Solution
    x: 2
    arr: [null, 1]
***/
var 1..2: x;
array[1..2] of var opt 1..1: arr;
constraint absent(arr[x - 1]) \/ absent(arr[x + 1]);
