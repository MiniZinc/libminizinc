%-----------------------------------------------------------------------------%
% Requires at least 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_at_least_set(int: n, array [int] of var set of int: xs, set of int: value) =
  sum (i in index_set(xs)) (xs[i] == value) >= n;
