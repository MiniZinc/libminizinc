%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_at_most_set_reif(
  int: n,
  array [int] of var set of int: xs,
  set of int: value,
  var bool: b,
) = b <-> sum (i in index_set(xs)) (xs[i] == value) <= n;
