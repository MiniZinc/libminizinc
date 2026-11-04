/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    c: false
    x: null
    d: false
    y: null
  - !Solution
    c: true
    x: 2
    d: false
    y: null
***/

% The bounds of the if-then-else branches restrict the domain of the optional variable. A singleton
% domain used to fix x to 2 (losing the absent branch), and an empty domain made the model fail
% instead of making y absent.
var bool: c :: add_to_output;
var opt 1..2: x :: add_to_output = if c then 2 else <> endif;
var bool: d :: add_to_output;
var opt 1..2: y :: add_to_output = if d then 3 else <> endif;
