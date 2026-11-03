/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
  regex: .*variable `z' must be defined, since it has par fields.*
***/

% An ann field is a par field as well (unlike an undefined ann declaration, which declares an
% annotation, it has no meaning without a value)
tuple(var bool, ann): z;
solve satisfy;
