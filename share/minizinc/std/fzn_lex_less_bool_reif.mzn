%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_bool_reif(array[int] of var bool: x,
                                 array[int] of var bool: y,
                                 var bool: c) =
  c <-> lex_less_std_decomposition(x,y);

%-----------------------------------------------------------------------------%
