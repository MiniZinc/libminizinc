include "fzn_exactly_int.mzn";
include "fzn_exactly_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate exactly_int(int: n, array[int] of var int: x, int: v) =
    fzn_exactly_int(n, x, v);
