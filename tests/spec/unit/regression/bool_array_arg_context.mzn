/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    x: 6
    q: false
  - !Solution
    x: 7
    q: false
  - !Solution
    x: 5
    q: true
  - !Solution
    x: 6
    q: true
  - !Solution
    x: 7
    q: true
  - !Solution
    x: 8
    q: true
***/

% The elements of a Boolean array argument used to be flattened in the context of the call, so
% they were half-reified here although xorall uses them in both polarities, and x = 8 was allowed
% with q false
var 5..8: x;
var bool: q;
constraint xorall([x > 5, x > 7]) \/ q;
