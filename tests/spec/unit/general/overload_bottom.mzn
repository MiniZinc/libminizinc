/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: !!set {}
***/

function var set of int: foo(array [int] of var set of int: x) = {};
function var bool: foo(array [int] of var bool: x) = true;

function array [int] of var set of int: empty() = [];
var set of int: x :: output = foo(empty());
