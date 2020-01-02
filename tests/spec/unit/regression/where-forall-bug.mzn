/***
!Test
expected: !Result
  status: SATISFIED
  solution: !Solution
    b: true
***/

% Regression test for a problem in mzn2fzn where failure of the
% where clause was incorrectly being treated as equivalent to
% model unsatisfiability.

var bool: b :: add_to_output = true;

constraint
    forall(c in 1..5 where forall([c mod 2 = 0])) (
        c >= 0
    );

solve satisfy;
