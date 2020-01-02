/***
!Test
expected: !Result
  status: SATISFIED
***/

annotation simpleann;
annotation composeann(ann: s) = seq_search([s,s]);
function ann: annfunc(ann: s) = composeann(s);
solve :: annfunc(composeann(simpleann)) satisfy;
