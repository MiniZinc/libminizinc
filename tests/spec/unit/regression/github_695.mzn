/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: -3
    y: -3
    idx: 0
    val: -3
    p: true
  - !Solution
    x: -3
    y: -3
    idx: 1
    val: -3
    p: true
  - !Solution
    x: -3
    y: -3
    idx: 2
    val: -3
    p: false
  - !Solution
    x: -3
    y: -3
    idx: 2
    val: -3
    p: true
***/

var -3..-3: x;
var -3..-3: y;
var 0..2: idx;
var -3..-2: val;
var bool: p;
constraint ([0: x, y][idx] = val) -> p;
constraint val = max([x,y]);
