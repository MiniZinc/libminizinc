% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% The reified form of `fzn_lex_less_float.mzn`, under the registry's name.

include "fzn_float_lex_less_reif.mzn";

predicate fzn_lex_less_float_reif(
  array [int] of var float: xs,
  array [int] of var float: ys,
  var bool: b,
) =
  fzn_float_lex_less_reif(array1d(xs), array1d(ys), b);
