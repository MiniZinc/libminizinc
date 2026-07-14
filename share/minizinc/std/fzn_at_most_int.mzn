include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_at_most_int(int: n, array [int] of var int: xs, int: value) = count(xs, value) <= n;
