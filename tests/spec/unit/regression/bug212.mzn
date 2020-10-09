/***
!Test
solvers: [gecode, chuffed]
expected:
- !Result
  solution: !Solution {}
***/

% A regression test for bug #212.  In r13904, the FlatZinc optimizer was
% aborting on the string literal annotation argument below.

annotation f(string:x);

var int : x;
var int : y;
constraint (x != y) :: f("abc");

solve satisfy;
