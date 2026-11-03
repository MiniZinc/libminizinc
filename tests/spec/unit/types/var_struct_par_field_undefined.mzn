/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*variable `z' must be defined, since it has par fields.*
***/

% The par field of an undefined var tuple has no value (this used to fail with an evaluation error
% about an introduced variable, or with a syntax error in the output)
tuple(var bool, int): z;
constraint z.2 = 4;
