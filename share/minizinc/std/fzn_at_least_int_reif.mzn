include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_at_least_int_reif(int: n, array[int] of var int: x, int: v, var bool: b) =
    b <-> count(x, v) >= n;
