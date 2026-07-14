%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_float(array [int] of var float: xs, array [int] of var float: ys) =
  lex_less_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%
