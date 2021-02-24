include "fzn_lex_chain_less_bool.mzn";
include "fzn_lex_chain_less_bool_reif.mzn";

%-----------------------------------------------------------------------------%
% Requires that the columns of matrix \a a are lexicographically sorted,
% strictly increasing.
%-----------------------------------------------------------------------------%

predicate lex_chain_less_bool(array[int, int] of var bool: a) =
    fzn_lex_chain_less_bool(a);

%-----------------------------------------------------------------------------%
