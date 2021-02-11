%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

include "fzn_lex_lesseq_int.mzn";

predicate fzn_lex_lesseq_bool(array[int] of var bool: x,
                              array[int] of var bool: y) =
    fzn_lex_lesseq_int(x, y);

%-----------------------------------------------------------------------------%
