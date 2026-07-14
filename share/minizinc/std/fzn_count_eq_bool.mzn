%-----------------------------------------------------------------------------%
% Constrains 'c' to be the number of occurrences of 'y' in 'x'.
%-----------------------------------------------------------------------------%

predicate fzn_count_eq_bool(array [int] of var bool: xs, var bool: value, var int: n) =
  n = sum (x_i in xs) (x_i == value);

%-----------------------------------------------------------------------------%
