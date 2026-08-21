% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% Disequality over floats.

predicate fzn_float_ne(var float: a, var float: b) = a != b;
