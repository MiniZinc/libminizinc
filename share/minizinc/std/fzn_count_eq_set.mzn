%-----------------------------------------------------------------------------%
% Constrains 'n' to be the number of occurrences of 'value' in 'xs'.
%-----------------------------------------------------------------------------%

predicate fzn_count_eq_set(
  array [int] of var set of int: xs,
  var set of int: value,
  var int: n,
) = n = sum (x_i in xs) (x_i == value);

%-----------------------------------------------------------------------------%
