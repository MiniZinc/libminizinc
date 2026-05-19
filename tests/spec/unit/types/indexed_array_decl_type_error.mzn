/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

% The expression after `in` in an indexed array declaration must be a set
% expression (e.g. `1..n`, `{1,2,3}`, a set parameter).  A bare type such as
% `int` is rejected at parse time, since it carries no domain to iterate over.
array[c in int] of var 1..c: x = [3,4,5];

solve satisfy;
