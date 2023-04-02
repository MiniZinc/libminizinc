/***
!Test
solver: [gecode]
expected: !Result
  status: UNSATISFIABLE
***/

% In 2.7.1 bind was accidentally changed to only consider bounds of A not the actual domain

var {0, 3}: A;
var 1..2: B;
constraint abs(B)=A;
