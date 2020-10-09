/***
!Test
expected: !Result
  status: SATISFIED
***/

constraint 0xff = 255;
constraint 0o377 = 255;
solve satisfy;
