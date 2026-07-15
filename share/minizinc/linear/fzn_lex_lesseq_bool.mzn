%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

include "fzn_lex_lesseq_int.mzn";

predicate fzn_lex_lesseq_bool(array [int] of var bool: xs, array [int] of var bool: ys) =
  fzn_lex_lesseq_int(xs, ys);

%-----------------------------------------------------------------------------%
