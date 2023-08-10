/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x:
      a: [[[1]]]
      b: 1
    y:
      a: [[[1]]]
      b: 1
***/

record(array [1..1] of tuple(array [1..1] of var 1..1): a, var 1..1: b): x;
record(array [1..1] of tuple(array [1..1] of int): a, int: b): y :: output = fix(x);

constraint assert(is_fixed(x), "Should be fixed");
