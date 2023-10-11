/***
!Test
expected: !Result
  solution: !Solution
    x: 1
***/

predicate foo(var int: x) = bar(x);
predicate bar(var int: x) = true;
predicate bar(int: x) = true;
var 1..3: x;
output [show(foo(x)), "\n"];
solve minimize x;
