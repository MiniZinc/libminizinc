include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_int(int: n, array[int] of var int: x, int: v) =
    n == count(x, v);
