/***
!Test
solvers: [gecode]
options:
  -G: std
expected: !Result
  status: UNSATISFIABLE
***/

% bind previously created constraint bool_clause_reif([a, b]), [], c) :: defines_var(c);
% which flattened into constraint exists([a, b]) :: defines_var(c);
% and then optimisation incorrectly removed the exists constraint due to c being fixed.

var bool: a;
var bool: b;
var bool: c;

var 0..1: x;
constraint c <-> x < 1;
constraint x <= 0;
constraint c = exists([a, b]);
constraint a < 1;
constraint b < 1;
