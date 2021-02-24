include "fzn_lex_chain_lesseq_int.mzn";

predicate fzn_lex_chain_lesseq_bool(array[int, int] of var bool: a) =
    fzn_lex_chain_lesseq_int(a);
