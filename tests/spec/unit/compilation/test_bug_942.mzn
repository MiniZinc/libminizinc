/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
***/

function list of var int: func(var int: x) = [x div x];
any: test_var ::output = [func(1)];
