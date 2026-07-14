%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_set(array [int] of var set of int: xs, array [int] of var set of int: ys) =
  lex_less_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%
