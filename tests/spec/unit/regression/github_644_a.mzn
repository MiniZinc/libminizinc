/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    arr: [null, null]
  - !Solution
    arr: [null, false]
  - !Solution
    arr: [null, true]
  - !Solution
    arr: [false, null]
  - !Solution
    arr: [false, false]
  - !Solution
    arr: [false, true]
  - !Solution
    arr: [true, true]
***/

array[1..2] of var opt bool: arr;

constraint arr[1] = true -> arr[2] = true;
