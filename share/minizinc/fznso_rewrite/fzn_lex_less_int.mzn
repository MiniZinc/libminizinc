% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_lex_less_int`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_int_lex_less.mzn";

predicate fzn_lex_less_int(
  array [int] of var int: xs :: promise_ctx_antitone,
  array [int] of var int: ys :: promise_ctx_monotone,
) =
  fzn_int_lex_less(array1d(xs), array1d(ys));
