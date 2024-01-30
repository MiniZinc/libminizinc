/***
!Test
solvers: [gecode]
options: 
  -G: std
expected: !Result
  status: UNSATISFIABLE
***/

% Regression test for where redefinition of bool_clause_reif was used for the
% known true exists([x1, x2]) causing a circular redefinition into
% clause([x1, x2], [true]) removing the constraint due to CSE.

var 0..2: x;
var bool: x1 :: output = x = 1;
var bool: x2 :: output = x = 2;
var bool: b = true;
var bool: c :: output = forall (i in {1} where b) (x1 \/ x2);
constraint c;
constraint x = 0;
