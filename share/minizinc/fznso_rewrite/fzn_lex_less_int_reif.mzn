% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_lex_less_int.mzn`, under the registry's name.

include "fzn_int_lex_less_reif.mzn";

predicate fzn_lex_less_int_reif(
  array [int] of var int: xs,
  array [int] of var int: ys,
  var bool: b,
) =
  fzn_int_lex_less_reif(array1d(xs), array1d(ys), b);
