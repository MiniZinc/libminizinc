/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x_int: 1
    y_int: 2
***/

var int: x_int;
constraint x_int = 1;
var float: x_float = int2float(x_int);
var int: y_int;
var float: y_float = int2float(y_int);
constraint y_float = x_float + 1.0;
