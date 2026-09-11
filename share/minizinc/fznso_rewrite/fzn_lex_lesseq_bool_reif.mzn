% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_lex_lesseq_bool.mzn`, under the registry's name.

include "fzn_bool_lex_lesseq_reif.mzn";

predicate fzn_lex_lesseq_bool_reif(
  array [int] of var bool: xs,
  array [int] of var bool: ys,
  var bool: b,
) =
  fzn_bool_lex_lesseq_reif(array1d(xs), array1d(ys), b);
