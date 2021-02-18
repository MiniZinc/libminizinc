include "fzn_lex_lesseq_bool.mzn";
include "fzn_lex_lesseq_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the array 'x' is lexicographically less than or equal to
% array 'y'.  Compares them from first to last element, regardless of indices
%-----------------------------------------------------------------------------%

predicate lex_lesseq_bool(array[int] of var bool: x ::promise_ctx_antitone,
                          array[int] of var bool: y ::promise_ctx_monotone) =
    fzn_lex_lesseq_bool(x, y);

predicate lex_leq_bool(array[int] of var bool: x ::promise_ctx_antitone,
                       array[int] of var bool: y ::promise_ctx_monotone) =
    lex_lesseq_bool(x, y);

%-----------------------------------------------------------------------------%
