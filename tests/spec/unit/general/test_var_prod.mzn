/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    p: 1
    xs: [1, 1, 1, 1]
- !Result
  status: SATISFIED
  solution: !Solution
    p: 140
    xs: [2, 7, 10, 1]
- !Result
  status: SATISFIED
  solution: !Solution
    p: 700
    xs: [10, 10, 7, 1]
- !Result
  status: SATISFIED
  solution: !Solution
    p: 1000
    xs: [1, 10, 10, 10]
- !Result
  status: SATISFIED
  solution: !Solution
    p: 1000
    xs: [10, 10, 1, 10]
***/

array [1..4] of var 1..10: xs :: add_to_output;
var 0..1000: p :: add_to_output = product(xs);
solve satisfy;
