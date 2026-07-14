%-----------------------------------------------------------------------------%
% Constrains 'n' to be the number of occurrences of 'value' in 'xs'.
%-----------------------------------------------------------------------------%

predicate fzn_count_eq_float(array [int] of var float: xs, var float: value, var int: n) =
  n = sum (x_i in xs) (x_i == value);

%-----------------------------------------------------------------------------%
