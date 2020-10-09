/***
!Test
expected: !Result
  status: SATISFIED
***/

constraint  7  mod  4 =  3;
constraint -7  mod  4 = -3;
constraint  7  mod -4 =  3;
constraint -7  mod -4 = -3; 

solve satisfy;

