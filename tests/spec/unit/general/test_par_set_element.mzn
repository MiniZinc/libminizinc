/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    a:
    - !!set {1}
    - !Range 1..2
    - !Range 1..3
    s: !!set {1}
    x: 1
- !Result
  status: SATISFIED
  solution: !Solution
    a: [!!set {1}, !Range 1..2, !Range 1..3]
    s: !Range 1..2
    x: 2
***/

array [1..3] of set of int: a :: add_to_output = [{1}, {1, 2}, {1, 2, 3}];
var 1..2: x :: add_to_output;
var set of 1..10: s :: add_to_output = a[x];
solve satisfy;
