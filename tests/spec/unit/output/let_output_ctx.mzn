/***
!Test
solvers: [gecode]
options:
  all_solutions: true
expected: !Result
  status: ALL_SOLUTIONS
  solution: !SolutionSet
  - !Solution
    x: 2
    'b1@35.18-47': false
    'o@37.40-73': false
  - !Solution
    x: 3
    'b1@35.18-47': false
    'o@37.40-73': true
  - !Solution
    x: 4
    'b1@35.18-47': true
    'o@37.40-73': true
  - !Solution
    x: 5
    'b1@35.18-47': true
    'o@37.40-73': true
***/

% The output reads let variables with ::output from the solver, so they are
% fully reified even if they are only used positively. Otherwise x = 4 or x = 5
% could also be printed with b1 = false.

predicate int_le_imp(var int: a, var int: b, var bool: r);

var 0..5: x;

constraint let { var bool: b1 :: output = x > 3; var bool: b2 = x > 1 } in b1 \/ b2;
% o is read from the solver, so its definition, and b0 in it, are fully reified
constraint let { var bool: b0 = x > 2; var bool: o :: output = b0 /\ true; var bool: b2 = x > 0 } in o \/ b2;
