%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_float_reif(array[int] of var float: x,
                                  array[int] of var float: y,
                                  var bool: c) =
  c <-> lex_less_std_decomposition(x,y);

%-----------------------------------------------------------------------------%
