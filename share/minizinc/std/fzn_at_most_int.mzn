%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

include "count_eq.mzn";


predicate fzn_at_most_int(int: n, array[int] of var int: x, int: v) =
    count_eq(x,v) <= n;

