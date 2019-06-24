include "fzn_lex_lesseq_set.mzn";
include "fzn_lex_lesseq_set_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_lesseq_set(array[int] of var set of int: x,
                         array[int] of var set of int: y) =
    fzn_lex_lesseq_set(x, y);

predicate lex_lesseq_set_reif(array[int] of var set of int: x,
                              array[int] of var set of int: y,
                              var bool: b) =
    fzn_lex_lesseq_set_reif(x, y, b);

predicate lex_leq_set(array[int] of var set of int: x,
                      array[int] of var set of int: y) =
    lex_lesseq_set(x, y);

%-----------------------------------------------------------------------------%
