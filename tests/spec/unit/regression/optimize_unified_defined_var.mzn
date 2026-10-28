/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  solution: !SolutionSet
  - !Solution
    y: 2
***/

% The optimiser unifies ov with the variable defined by y = 2. When ov was then removed, the
% variable it had been unified with was removed instead, together with its definition.
var bool: c;
var 0..5: y :: add_to_output;
var bool: oc;
var bool: ov;
constraint c;
constraint c -> (oc /\ (ov <-> y = 2));
constraint oc -> ov;
