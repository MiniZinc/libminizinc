% RUNS ON mzn_mer_fd

% Regression test for a problem in mzn2fzn where failure of the
% where clause was incorrectly being treated as equivalent to
% model unsatisfiability.

var bool: b = true;

constraint
    forall(c in 1..5 where forall([c mod 2 = 0])) (
        c >= 0
    );

solve satisfy;

output ["b = ", show(b), "\n"];
