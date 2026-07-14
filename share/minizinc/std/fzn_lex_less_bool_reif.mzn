%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is strictly lexicographically less than array 'ys'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_bool_reif(
  array [int] of var bool: xs,
  array [int] of var bool: ys,
  var bool: c,
) = c <-> lex_less_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%
