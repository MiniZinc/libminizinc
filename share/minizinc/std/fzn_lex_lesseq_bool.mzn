%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate fzn_lex_lesseq_bool(array[int] of var bool: x ::promise_ctx_antitone,
                              array[int] of var bool: y ::promise_ctx_monotone) =
  lex_lesseq_std_decomposition(x,y);

%-----------------------------------------------------------------------------%
