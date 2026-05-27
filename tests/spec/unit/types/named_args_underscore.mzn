/***
!Test
solvers: [gecode]
expected: !Error
  type: SyntaxError
  regex: "parameter `_x' starts with '_' and cannot be used as a named argument"
type: compile
***/

% Parameter names starting with `_' are not part of the public API: they may
% appear in declarations and the function body, but cannot be supplied as a
% named argument at the call site. This opt-out mechanism lets the standard
% library defer naming decisions without locking in parameter names.

predicate foo(int: _x, int: y);
constraint foo(_x: 1, y: 2);

solve satisfy;
