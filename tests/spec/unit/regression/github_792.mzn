/***
!Test
solvers: [gecode]
options: 
  -G: std
expected:
- !Result
  solution: !Solution
    y: true
- !Result
  solution: !Solution
    y: false
***/

array[1..2] of tuple(var 1..2): x :: no_output;
function var bool: foo() = (1,) in x;
var bool: y :: output = foo();
