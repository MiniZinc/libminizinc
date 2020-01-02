/***
!Test
expected: !Result
  solution: !Solution
    a:
    - !!set {1}
    - !Range 2..3
    - !Range 4..5
***/

array [1..3] of var set of 1..5: a :: add_to_output = [{1}, {2, 3}, {4, 5}];
solve satisfy;