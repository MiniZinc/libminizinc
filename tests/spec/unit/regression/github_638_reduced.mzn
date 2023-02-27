/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    arr: [false, true]
    x : [null]
    xx : 1
  - !Solution
    arr: [false, true]
    x : [null]
    xx : 2
  - !Solution
    arr: [true, true]
    x : [2]
    xx : 2
***/
array[1..2] of var bool: arr;
constraint arr[2];
any: x ::output= [if forall(j in 1..i)(arr[j]) then i else <> endif| i in 2..2];
constraint occurs(x[1]) -> xx = 2;
var 1..2: xx ::output;
