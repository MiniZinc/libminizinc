/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 1
    y: 3
  - !Solution
    x: 2
    y: 4
  - !Solution
    x: 3
    y: 4
***/

% A Boolean argument of a user-defined operator with a non-Boolean result used to be flattened in
% the context of the whole expression, so here x > 1 became a top-level constraint
function var int: '+'(var bool: a, string: s) = bool2int(a) + string_length(s);
var 1..3: x;
var int: y :: output = (x > 1) + "abc";
