/***
!Test
expected:
- !Error
  type: TypeError
type: compile
***/

% An unbounded-above array index set must start at 1 (only `1..infinity', i.e. a `list',
% is allowed). A hand-written `array[3..infinity]' is rejected.
function int: f(array[3..infinity] of int: x) = x[3];
int: r = f([10, 20, 30]);
solve satisfy;
