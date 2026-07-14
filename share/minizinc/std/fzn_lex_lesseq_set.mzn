%-----------------------------------------------------------------------------%
% Requires that the array 'xs' is lexicographically less than or equal to
% array 'ys'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_set(array [int] of var set of int: xs, array [int] of var set of int: ys) =
  lex_lesseq_std_decomposition(xs, ys);

%-----------------------------------------------------------------------------%
