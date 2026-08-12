/***
!Test
solvers: [gecode]
type: compile
expected: !Error
  message: 'array access using a variable is not supported for arrays which contain other arrays'
  type: TypeError
***/

% Projecting an array-valued field yields an array of arrays (see
% struct_array_projection_array_field.mzn), whose inner arrays may have different index sets. It
% must therefore inherit the existing restriction that such an array cannot be indexed by a
% variable, since the index set of the result would depend on the variable's value.
%
% This holds by construction rather than by accident: the projection builds the same marker-tuple
% representation that vComprehension builds, so `vArrayAccess' sees a struct element type whose
% field has dim() != 0 and its `containsArray' guard fires. The hand-written equivalent
% `[x[j].1 | j in 1..2][i]' is rejected at exactly the same point.

any: x = [([1, 2], 3), ([4, 5, 6], 7)];
var 1..2: i;
any: y :: output = x.1[i];
