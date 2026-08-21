% The constraint `bool_lex_chain_lesseq_orbitope_packing_reif`, as the FZnSO
% registry defines it.
%
% This file is a solver's override point: declare
% `bool_lex_chain_lesseq_orbitope_packing_reif` and the body below is dropped in
% favour of the propagator, or ship a file of this name with a decomposition of
% your own.

include "fzn_bool_lex_chain_lesseq.mzn";

predicate fzn_bool_lex_chain_lesseq_orbitope_packing_reif(list of var bool: xs, int: columns, var bool: b) =
  b <-> (
    fzn_bool_lex_chain_lesseq(xs, columns) /\
      forall (i in 1..length(xs) div columns) (
        sum (j in 1..columns) (xs[(i - 1) * columns + j]) <= 1
      )
  );
