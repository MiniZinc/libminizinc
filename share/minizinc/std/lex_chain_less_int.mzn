include "fzn_lex_chain_less_int.mzn";
include "fzn_lex_chain_less_int_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the columns of matrix \a a are lexicographically sorted,
% strictly increasing.
%-----------------------------------------------------------------------------%

predicate lex_chain_less_int(array[int, int] of var int: a) =
    fzn_lex_chain_less_int(a);

%-----------------------------------------------------------------------------%
