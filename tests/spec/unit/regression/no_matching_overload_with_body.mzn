/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: '.*no function or predicate with this signature found: `f\(string\)''.*function int : f\(int: x\);.*function int : f\(float: x,int: y\);.*'
***/

% The error message lists the candidate overloads without their bodies, which are detached while
% they are printed and must survive being put back.

function int: f(int: x) = x + 1;
function int: f(float: x, int: y) = y + 2;
var 0..3: v;
constraint f("a") = v;
