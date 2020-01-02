/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    X: 6
- !Result
  status: SATISFIED
  solution: !Solution
    X: 7
- !Result
  status: SATISFIED
  solution: !Solution
    X: 8
- !Result
  status: SATISFIED
  solution: !Solution
    X: 9
***/

var 0..9: X;
constraint X > 5;
solve satisfy;
