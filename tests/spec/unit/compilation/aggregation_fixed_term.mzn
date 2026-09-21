/***
--- !Test
type: compile
solvers: [gecode]
expected: !FlatZinc aggregation_fixed_term.fzn
***/

% Expect y to disappear and its value to shift both bounds because its singleton
% domain is substituted before terms are counted.

array [1..3] of var 0..10: a;
var 0..10: y;
var bool: p;

constraint y = 3;
constraint (a[1] + a[2] + y >= 4) \/ p;
constraint a[1] + a[2] + a[3] + y <= 9;
