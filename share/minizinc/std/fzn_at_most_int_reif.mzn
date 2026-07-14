include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_at_most_int_reif(int: n, array [int] of var int: xs, int: value, var bool: b) =
  b <-> count(xs, value) <= n;
