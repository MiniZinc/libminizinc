%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_set(array[int] of var set of int: x,
                             array[int] of var set of int: y) =
  lex_lesseq_std_decomposition(x,y);

%-----------------------------------------------------------------------------%
