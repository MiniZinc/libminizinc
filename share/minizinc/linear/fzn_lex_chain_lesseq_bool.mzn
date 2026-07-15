include "fzn_lex_chain_lesseq_int.mzn";

predicate fzn_lex_chain_lesseq_bool(array [int, int] of var bool: xs) = fzn_lex_chain_lesseq_int(xs);
