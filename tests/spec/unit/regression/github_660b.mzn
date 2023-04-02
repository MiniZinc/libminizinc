/***
!Test
solver: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

% In 2.7.1 bind was accidentally changed to only consider bounds of A not the actual domain

var {0.0, 3.0}: A;
var 1.0..2.0: B;
constraint abs(B)=A;
