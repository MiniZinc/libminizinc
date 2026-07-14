include "count_fn.mzn";

%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_int_reif(int: n, array [int] of var int: xs, int: value, var bool: b) =
  b <-> n == count(xs, value);
