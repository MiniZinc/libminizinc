/***
!Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x: 1
    y: 2
***/
predicate foo(var 1..: i) = true;
var 1..2: x;
constraint not foo(x - 1);

predicate bar(var ..-1 union {1,3} union 5..: i) = true;
var 2..3: y;
constraint not bar(y);