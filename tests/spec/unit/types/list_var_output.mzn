/***
!Test
solvers: [gecode]
expected: !Result
  status: SATISFIED
  solution: !Solution
    x: [true]
    y: [1, 2]
***/

% A `list of var T' declaration in the output: the output annotation must use the concrete index
% set of the flat array, not the `1..infinity' list marker.
list of var bool: x ::output = let { var 1..3: x; list of var bool: y = [x < 5] } in [y[1]];
list of var int: y ::output = let { var 1..1: z } in [z, z + 1];
