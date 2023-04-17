%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_float(array[int] of var float: x,
                               array[int] of var float: y) =
  lex_lesseq_std_decomposition(x,y);
%-----------------------------------------------------------------------------%
