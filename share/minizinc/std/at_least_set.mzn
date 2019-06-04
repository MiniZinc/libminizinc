include "fzn_at_least_set.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_least_set(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_least_set(n, x, v)
