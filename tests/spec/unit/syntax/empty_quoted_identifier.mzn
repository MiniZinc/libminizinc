/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
***/

% An empty quoted identifier is not a name. It used to be lowered to the same
% empty string the lowering uses to signal failure, so the item was dropped
% without a diagnostic.
int: x = 1;
int: '' = 2;
solve satisfy;
