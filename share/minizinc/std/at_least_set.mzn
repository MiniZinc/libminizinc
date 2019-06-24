include "fzn_at_least_set.mzn";
include "fzn_at_least_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_least_set(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_least_set(n, x, v);

predicate at_least_set_reif(int: n, array[int] of var set of int: x, set of int: v, var bool: b) =
    fzn_at_least_set_reif(n, x, v, b);
