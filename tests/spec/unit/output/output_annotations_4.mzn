/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    a: 9
    y@11.3-24|foo@15.12-17: 10
***/

predicate foo(var int: x) = let {
  var 1..10: y :: output;
} in x < y;

var 1..20: a;
constraint foo(a);

solve maximize a;
