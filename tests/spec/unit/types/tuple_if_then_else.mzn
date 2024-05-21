/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    b: true
    p0: [3, 4]
  - !Solution
    b: false
    p0: [3, 5]
***/

var bool: b;
var tuple(int, int): p0;
tuple(int, int): p1 = (3, 4);
tuple(int, int): p2 = (3, 5);
constraint if b then p0 = p1 else p0 = p2 endif;
