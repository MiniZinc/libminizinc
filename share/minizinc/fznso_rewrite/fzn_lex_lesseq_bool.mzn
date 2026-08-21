% Rewrites `std`'s spelling into the registry's. Nothing here is a solver's
% concern — the constraint itself lives in `fznso_constraints/`.

% `std` spells this `fzn_lex_lesseq_bool`; the registry names it for the type it
% takes and puts that first. The predicate a solver implements is the registry's;
% the overload `std` calls forwards to it.

include "fzn_bool_lex_lesseq.mzn";

predicate fzn_lex_lesseq_bool(
  array [int] of var bool: xs :: promise_ctx_antitone,
  array [int] of var bool: ys :: promise_ctx_monotone,
) =
  fzn_bool_lex_lesseq(array1d(xs), array1d(ys));
