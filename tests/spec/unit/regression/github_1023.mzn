/***
!Test
expected: !Result
  status: UNSATISFIABLE
***/

% Constraining a non-optional variable to be equal to an absent optional
% parameter value used to crash the flattener (segmentation fault). It should
% instead be detected as unsatisfiable, since a non-optional variable can never
% take the absent value `<>`.

opt int: d = <>;
var 1..3: x;
constraint x = d;
solve satisfy;
