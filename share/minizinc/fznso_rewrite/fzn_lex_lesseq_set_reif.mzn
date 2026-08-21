% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_lex_lesseq_set.mzn`, under the registry's name.

include "fzn_set_of_int_lex_lesseq_reif.mzn";

predicate fzn_lex_lesseq_set_reif(
  array [int] of var set of int: xs,
  array [int] of var set of int: ys,
  var bool: b,
) =
  fzn_set_of_int_lex_lesseq_reif(array1d(xs), array1d(ys), b);
