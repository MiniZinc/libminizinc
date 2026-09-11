% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form lives in its own file, as every reified form does, so that a
% solver overriding the plain constraint does not have to carry a copy of this
% one along with it.

predicate fzn_float_ne_reif(var float: a, var float: b, var bool: r) = r <-> a != b;
