/***
!Test
solvers: [gecode]
options:
  keep-paths: true
expected: !Result
  solution: !Solution
    a: 2
    b: 1
    c: [[1, 2], [3, 4]]
***/

var 2..4: a;

function int: foo(tuple(int, int): x, tuple(int, int): y) = 1;

any: b :: output = foo((1, lb(a)), (3, ub(a)));
any: c :: output = [(1, lb(a)), (3, ub(a))];

solve minimize a;
