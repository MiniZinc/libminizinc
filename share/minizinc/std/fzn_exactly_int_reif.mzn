include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_int_reif(int: n, array[int] of var int: x, int: v, var bool: b) =
    b <-> n == count(x, v);
