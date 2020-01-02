/***
!Test
expected: !Result
  status: SATISFIED
***/

constraint  7  div  4 =  1;
constraint -7  div  4 = -1;
constraint  7  div -4 = -1;
constraint -7  div -4 =  1;

solve satisfy;
