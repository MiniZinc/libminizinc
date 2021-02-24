include "fzn_lex_chain_lesseq_bool.mzn";
include "fzn_lex_chain_lesseq_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the columns of matrix \a a are lexicographically sorted,
% non-decreasing.
%-----------------------------------------------------------------------------%

predicate lex_chain_lesseq_bool(array[int, int] of var bool: a) =
    fzn_lex_chain_lesseq_bool(a);

%-----------------------------------------------------------------------------%
