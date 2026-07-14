%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is lexicographically less than or equal to
% array 'ys'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_float(array [int] of var float: xs, array [int] of var float: ys) =
  lex_lesseq_std_decomposition(xs, ys);
%-----------------------------------------------------------------------------%
