/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: true
    b: false
***/

var bool: a;
var bool: b;

predicate foo(var bool: p);
predicate foo_reif(var bool: p, var bool: q) = p <-> q;

constraint foo(a) != b;
constraint a;
