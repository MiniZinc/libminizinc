include "fzn_exactly_set.mzn";
include "fzn_exactly_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate exactly_set(int: n, array[int] of var set of int: x, set of int: v) =
    fzn_exactly_set(n, x, v);
