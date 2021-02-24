include "fzn_lex_chain_lesseq_int.mzn";
include "fzn_lex_chain_lesseq_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the columns of matrix \a a are lexicographically sorted,
% non-decreasing.
%-----------------------------------------------------------------------------%

predicate lex_chain_lesseq_int(array[int, int] of var int: a) =
    fzn_lex_chain_lesseq_int(a);

%-----------------------------------------------------------------------------%
