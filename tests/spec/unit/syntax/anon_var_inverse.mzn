/***
!Test
solvers: [gecode]
expected: !Error
  type: TypeError
***/

% `_^-1` keeps the inverse operator (parser.yxx:1590); only `<>^-1` drops it
% (:1618). Applying `^` to `_` has no matching operator, so this is a type
% error. Dropping the operator would leave a bare `_` and make it type-check.
var 1..3: x;
constraint x = _^-1;

solve satisfy;
