%-----------------------------------------------------------------------------%
% Constrains 'c' to be the number of occurrences of 'y' in 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_count_eq_set(array [int] of var set of int: x, var set of int: y, var int: c) =
  c = sum (x_i in x) (x_i == y);

%-----------------------------------------------------------------------------%
