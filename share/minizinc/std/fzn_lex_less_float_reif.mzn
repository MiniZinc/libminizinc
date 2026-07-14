%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is strictly lexicographically less than array 'ys'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_float_reif(
  array [int] of var float: xs,
  array [int] of var float: ys,
  var bool: c,
) = c <-> lex_less_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%
