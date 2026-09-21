/***
!Test
solvers: [cbc]
expected: !Result
  status: UNSATISFIABLE
***/

% A relation delayed because an operand holds a linear sum still has to be
% decomposed by the solver library when that sum keeps its name.
var 1..3: f;
var 1..3: g;
var 1..3: c;
constraint f != g;
constraint f = g \/ g != c;
constraint g <= c /\ c <= g;
