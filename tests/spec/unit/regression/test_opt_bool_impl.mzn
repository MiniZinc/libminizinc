/***
!Test
expected:
- !Result
  status: SATISFIED
  solution: !Solution
    b: true
- !Result
  status: SATISFIED
  solution: !Solution
    b: false
***/

var bool: b;
constraint b -> <>;
