include "fzn_at_most_set.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_most_set(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_at_most_set(n, x, v);

