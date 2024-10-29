/***
!Test
expected:
- !Result
  status: SATISFIED
***/

var bool: b;
var opt bool: b_opt;
constraint exists([b, b_opt, b]);
constraint exists([b, <>]);
constraint forall([b, <>]);
