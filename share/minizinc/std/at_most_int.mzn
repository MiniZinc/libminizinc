include "fzn_at_most_int.mzn";
include "fzn_at_most_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate at_most_int(int: n, array[int] of var int: x, int: v) =
    fzn_at_most_int(n, x, v);

predicate at_most_int_reif(int: n, array[int] of var int: x, int: v, var bool: b) =
    fzn_at_most_int_reif(n, x, v, b);
