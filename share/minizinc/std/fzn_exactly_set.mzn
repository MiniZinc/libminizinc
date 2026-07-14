%-----------------------------------------------------------------------------%
% Requires exactly 'n' variables in 'xs' to take the value 'value'.
%-----------------------------------------------------------------------------%

predicate fzn_exactly_set(int: n, array [int] of var set of int: xs, set of int: value) =
  n == sum (i in index_set(xs)) (xs[i] == value);
