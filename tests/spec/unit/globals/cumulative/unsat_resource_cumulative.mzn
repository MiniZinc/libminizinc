/***
!Test
expected:
- !Result
  status: UNSATISFIABLE
***/

include "cumulative.mzn";
var 0..10: start;
constraint cumulative([start], [5], [5], 1);
