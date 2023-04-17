%-----------------------------------------------------------------------------%
% Requires that the array 'x' is strictly lexicographically less than array 'y'.
% Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_less_int_reif(array[int] of var int: x,
                                array[int] of var int: y,
                                var bool: c) =
  c <-> lex_less_std_decomposition(x,y);

%-----------------------------------------------------------------------------%
