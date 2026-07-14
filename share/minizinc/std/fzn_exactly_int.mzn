include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_int(int: n, array [int] of var int: xs, int: value) = n == count(xs, value);
