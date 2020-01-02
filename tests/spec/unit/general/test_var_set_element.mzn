/***
!Test
expected:
- !Result
  solution: !Solution
    a: [!!set {}, !!set {}, !!set {}]
    s: !!set {}
    x: 3
- !Result
  solution: !Solution
    a: [!Range 1..5, !Range 1..5, !Range 1..5]
    s: !Range 1..5
    x: 2
***/

array [1..3] of var set of 1..5: a :: add_to_output;
var 2..10: x :: add_to_output;
var set of 1..10: s :: add_to_output = a[x];
solve satisfy;
