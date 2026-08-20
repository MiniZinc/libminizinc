/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: 2
    y: 1
***/

function var bool: foo(var int: x, tuple(var int): y) = x = y.1;
var 1..3: a;
var 1..3: y;
constraint foo(a, (2,));
solve minimize y;
