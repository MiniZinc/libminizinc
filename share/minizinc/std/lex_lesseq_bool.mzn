include "lex_lesseq.mzn";
include "fzn_lex_lesseq_bool.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_lesseq_bool(array[int] of var bool: x,
                          array[int] of var bool: y) =
    fzn_lex_lesseq_bool(x, y);

predicate lex_lesseq_bool_reif(array[int] of var bool: x,
                               array[int] of var bool: y,
                               var bool: c) =
    fzn_lex_lesseq_bool_reif(x, y, c);

predicate lex_leq_bool(array[int] of var bool: x,
                       array[int] of var bool: y) =
    lex_lesseq(x, y);

%-----------------------------------------------------------------------------%
