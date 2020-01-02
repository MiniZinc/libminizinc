/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    bla: 2
- !Result
  status: SATISFIED
  solution: !Solution
    bla: 4
***/

var {2,4}: bla;
constraint not (bla in {1,3});
solve satisfy;
