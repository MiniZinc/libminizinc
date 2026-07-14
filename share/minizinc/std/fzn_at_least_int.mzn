include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_at_least_int(int: n, array [int] of var int: xs, int: value) = count(xs, value) >= n;
