/***
--- !Test
solvers: [gecode]
expected: !Result
  solution: !Solution
    x@10.3-24: 1
***/

constraint let {
  var 1..10: x :: output;
} in true;
