/***
!Test
solvers: [gecode, cbc, highs]
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    y: 0
- !Result
  status: SATISFIED
  solution: !Solution
    y: 1
***/

var 0..1: y; 

constraint not ( 2 div y + 2 div (y - 1) == 0 ); 

solve satisfy;
