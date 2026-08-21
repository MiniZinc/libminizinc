% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_lex_chain_lesseq_int.mzn`, under the registry's name.

include "fzn_int_lex_chain_lesseq_reif.mzn";

predicate fzn_lex_chain_lesseq_int_reif(array [int, int] of var int: xs, var bool: b) =
  fzn_int_lex_chain_lesseq_reif(array1d(xs), card(index_set_2of2(xs)), b);
