%-----------------------------------------------------------------------------%
% Requires at most 'n' variables in 'x' to take the value 'v'.
%-----------------------------------------------------------------------------%

predicate fzn_at_most_set(int: n, array [int] of var set of int: xs, set of int: value) =
  sum (i in index_set(xs)) (xs[i] == value) <= n;
